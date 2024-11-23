#include "stdafx.h"

#include "MacroEvaluatedCodeVerifier.h"

#include <iomanip>
#include <sstream>

#include <boost/variant.hpp>

#include "MessageTextImpl.h"
#include "util.h"

namespace parser {
namespace fanuc {

constexpr std::array<std::string_view, 9> words{"#", "=", "+", "-", "*", "/", ":", "N", "GOTO"};

class MacroEvaluatedCodeVisitor : public boost::static_visitor<>
{
public:
    MacroEvaluatedCodeVisitor(const std::vector<std::string>& allowed_operations, const macro_map& macro_values,
                              int line, ELanguage language)
        : allowed_operations(allowed_operations)
        , macro_values(macro_values)
        , line(line)
        , language(language)
    {
    }

    void operator()(const DecimalAttributeData& data)
    {
        std::string_view word_view = data.word;
        if (std::none_of(words.cbegin(), words.cend(), [&](auto value) { return word_view == value; }))
        {
            auto it = std::find_if(std::begin(allowed_operations), std::end(allowed_operations),
                                   [&](const std::string& s) { return data.word == s; });
            if (it == std::end(allowed_operations))
                was_interesting_word = true;
        }

        text += data.word;
        if (data.sign)
            text += *data.sign;
        if (data.open_bracket)
            text += *data.open_bracket;
        if (was_interesting_word && data.macro)
        {
            unsigned int val        = std::stoi(*data.value);
            auto         macro_itor = macro_values.lower_bound({val, line});
            if (macro_itor == std::end(macro_values) ||
                (macro_itor != std::end(macro_values) && macro_itor->first.id != val))
                throw macro_evaluated_code_exception(
                    make_message(MessageName::MissingValue, language, "#" + *data.value));
            text += to_string_trunc(macro_itor->second);
            was_modified = true;
        }
        else
        {
            if (data.value)
                text += *data.value;
            if (data.dot)
                text += *data.dot;
            if (data.value2)
                text += *data.value2;
        }
        if (data.close_bracket)
            text += *data.close_bracket;
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

    std::string get_text() const
    {
        return text;
    }

    bool modified() const
    {
        return was_modified;
    }

private:
    const std::vector<std::string>& allowed_operations;
    const macro_map&                macro_values;
    const int                       line;
    const ELanguage                 language;
    bool                            was_modified{};
    bool                            was_interesting_word{};
    std::string                     text;
};

std::string MacroEvaluatedCodeVerifier::operator()(const std::vector<std::string>& allowed_operations,
                                                   const macro_map& macro_values, int line,
                                                   const std::vector<AttributeVariant>& value, ELanguage language)
{
    MacroEvaluatedCodeVisitor macro_visitor(allowed_operations, macro_values, line, language);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(macro_visitor, value[x]);

    if (macro_visitor.modified())
        return macro_visitor.get_text();

    return "";
}

} // namespace fanuc
} // namespace parser
