#include "stdafx.h"

#include "UnitConverter.h"

#include <array>
#include <iomanip>
#include <sstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/variant.hpp>

#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

class UnitConverterVisitor : public boost::static_visitor<>
{
public:
    UnitConverterVisitor(const ConvWordGrammar& conv_word_grammar, const std::vector<std::string>& conv_words,
                         UnitConversionType conversion_type)
        : word_grammar(conv_word_grammar)
        , words(conv_words)
        , conversion_type(conversion_type)
    {
    }

    void operator()(DecimalAttributeData& data)
    {
        if (ignore_rest)
            return;

        if (!was_first_item)
        {
            if (data.word == "/")
            {
                ignore_rest = true;
                return;
            }
        }
        was_first_item = true;

        if (data.word == "G" && data.value && std::stoi(*data.value) == 4)
        {
            ignore_rest = true;
            return;
        }

        if (data.macro || data.open_bracket || data.close_bracket)
            return;

        if (data.word == "G" && data.value)
        {
            const auto val = std::stoi(*data.value);
            if (val == 20 && conversion_type == UnitConversionType::imperial_to_metric)
            {
                *data.value = "21";
                return;
            }
            else if (val == 21 && conversion_type == UnitConversionType::metric_to_imperial)
            {
                *data.value = "20";
                return;
            }
            else if (val == 68)
            {
                if (data.value2)
                {
                    auto val2 = std::stoi(*data.value2);
                    if (val2 == 2 || val2 == 3 || val2 == 4)
                    {
                        was_g68 = true;
                        return;
                    }
                }
                else
                {
                    was_g68 = true;
                    return;
                }
            }
        }

        if (was_g68)
        {
            auto itor = std::find_if(std::cbegin(gcode68_ignore_params), std::cend(gcode68_ignore_params),
                                     [&](auto value) { return data.word == value; });

            if (itor != std::cend(gcode68_ignore_params))
                return;
        }

        if (!std::any_of(std::begin(words), std::end(words),
                         [&](const std::string& item) { return item == data.word; }))
            return;

        std::string str_value;
        if (data.value)
            str_value += *data.value;
        if (data.dot)
            str_value += *data.dot;
        if (data.value2)
            str_value += *data.value2;

        double dvalue = std::stod(str_value);
        switch (conversion_type)
        {
        case UnitConversionType::metric_to_imperial:
            dvalue /= 25.4;
            break;
        case UnitConversionType::imperial_to_metric:
            dvalue *= 25.4;
            break;
        }

        size_t      precision{};
        const auto& word_settings = word_grammar.at(data.word);
        if (word_settings.decimal_to > 0)
        {
            const auto tmp(std::to_string(word_settings.decimal_to));
            precision = tmp.size();
        }

        std::ostringstream ostr;
        ostr << std::fixed << std::setprecision(precision) << dvalue;

        std::string ret_str(ostr.str());
        boost::algorithm::trim_right_if(ret_str, boost::is_any_of("0"));
        boost::algorithm::trim_right_if(ret_str, boost::is_any_of("."));

        const auto pos = ret_str.find('.');
        if (pos != std::string::npos)
        {
            data.value      = boost::make_optional(ret_str.substr(0, pos));
            data.dot        = boost::make_optional('.');
            const auto rest = ret_str.substr(pos + 1);
            if (!rest.empty())
                data.value2 = boost::make_optional(rest);
        }
        else
        {
            data.value  = boost::make_optional(ret_str);
            data.dot    = boost::none;
            data.value2 = boost::none;
        }
    }

    template <typename T>
    void operator()(T& data)
    {
    }

private:
    const ConvWordGrammar&          word_grammar;
    const std::vector<std::string>& words;
    const UnitConversionType        conversion_type;
    bool                            was_first_item{};
    bool                            ignore_rest{};
    bool                            was_g68{};

    static constexpr std::array<std::string_view, 4> gcode68_ignore_params = {"I", "J", "K", "R"};
};

UnitConverter::UnitConverter(const FanucWordGrammar& word_grammar)
    : word_grammar(word_grammar)
{
    cache_grammar(word_grammar.metric, conv_word_grammar_metric, conv_words_metric);
    cache_grammar(word_grammar.imperial, conv_word_grammar_imperial, conv_words_imperial);
}

void UnitConverter::cache_grammar(const word_map& wm, ConvWordGrammar& conv_word_grammar,
                                  std::vector<std::string>& conv_words)
{
    std::for_each(std::cbegin(wm), std::cend(wm), [&](const auto& wg) {
        if (wg.second.word_type == WordType::length || wg.second.word_type == WordType::feed)
        {
            conv_word_grammar.emplace(std::make_pair(wg.first, wg.second));
            conv_words.push_back(wg.first);
        }
    });
}

void UnitConverter::mm_to_inch(std::vector<AttributeVariant>& value)
{
    UnitConverterVisitor unit_converter_visitor(conv_word_grammar_imperial, conv_words_imperial,
                                                UnitConversionType::metric_to_imperial);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(unit_converter_visitor, value[x]);
}

void UnitConverter::inch_to_mm(std::vector<AttributeVariant>& value)
{
    UnitConverterVisitor unit_converter_visitor(conv_word_grammar_metric, conv_words_metric,
                                                UnitConversionType::imperial_to_metric);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(unit_converter_visitor, value[x]);
}

} // namespace fanuc
} // namespace parser
