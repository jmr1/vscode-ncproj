#include "stdafx.h"

#include "MacroEvaluator.h"

#include <boost/variant.hpp>

#include "MessageTextImpl.h"
#include "ValueCalculator.h"

namespace parser {
namespace fanuc {

class MacroVisitor : public boost::static_visitor<>
{
public:
    void operator()(const DecimalAttributeData& data)
    {
        if (!was_equal_sign)
        {
            if (data.word == "#")
            {
                if (data.value)
                    macro = std::stoi(*data.value);
                return;
            }
            else if (macro != -1)
            {
                if (data.word == "=")
                {
                    was_equal_sign = true;
                }
            }
            else
            {
                return;
            }
        }
        else
        {
            text += data.word;
        }

        if (data.sign)
            text += *data.sign;
        if (data.open_bracket)
            text += *data.open_bracket;
        if (data.macro)
            text += *data.macro;
        if (data.value)
            text += *data.value;
        if (data.dot)
            text += *data.dot;
        if (data.value2)
            text += *data.value2;
        if (data.close_bracket)
            text += *data.close_bracket;
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

    int get_macro() const
    {
        return macro;
    }

    std::string get_text() const
    {
        return text;
    }

private:
    int         macro = -1;
    bool        was_equal_sign{};
    std::string text;
};

bool MacroEvaluator::evaluate(macro_map& macro_values, int line, const std::vector<AttributeVariant>& value,
                              std::string& message, bool single_line_msg, ELanguage language)
{
    MacroVisitor macro_visitor;
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(macro_visitor, value[x]);
    const auto macro_id   = macro_visitor.get_macro();
    const auto macro_text = macro_visitor.get_text();

    if (macro_id != -1)
    {
        if (macro_text.empty())
        {
            message = make_message(MessageName::WrongMacroDefinition, language, std::to_string(macro_id));
            return false;
        }

        double          macro_result_value{};
        ValueCalculator value_calculator;
        auto vc_ret = value_calculator.parse(macro_text, message, single_line_msg, macro_values, macro_result_value, language);
        if (!vc_ret)
            return false;
        macro_values.emplace(std::make_pair(macro_map_key{static_cast<decltype(macro_map_key::id)>(macro_id), line},
                                            macro_result_value));
    }

    return true;
}

} // namespace fanuc
} // namespace parser
