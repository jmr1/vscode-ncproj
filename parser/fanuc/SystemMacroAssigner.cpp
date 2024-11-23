#include "stdafx.h"

#include "SystemMacroAssigner.h"

#include <boost/variant.hpp>

namespace parser {
namespace fanuc {

namespace {
std::unordered_map<std::string, std::vector<unsigned int>> word_macros_map = {
    {"B", {4102, 4302, 4502}}, {"D", {4107, 4307, 4507}}, {"E", {4108, 4308, 4508}},
    {"F", {4109, 4309, 4509}}, {"H", {4111, 4311, 4511}}, {"M", {4113, 4313, 4513}},
    {"N", {4114, 4314, 4514}}, {"O", {4115, 4315, 4515}}, {"S", {4119, 4319, 4519}},
};
}
class SystemMacroAssignerVisitor : public boost::static_visitor<>
{
public:
    SystemMacroAssignerVisitor(macro_map& macro_values, int line)
        : macro_values(macro_values)
        , line(line)
    {
    }

    void operator()(const fanuc::AttributeData<std::string>& data)
    {
        // Handles words which value is string like program name: O1234
        // Macro value is double floating point but program name although composed of digits it is a string
    }

    void operator()(const DecimalAttributeData& data)
    {
        auto value_extractor = [](const DecimalAttributeData& data) {
            std::string str;
            if (data.sign)
                str += *data.sign;
            if (data.value)
                str += *data.value;
            if (data.dot)
            {
                str += *data.dot;
                if (data.value2)
                    str += *data.value2;
            }
            return str;
        };

        auto macro_value_inserter = [&](const auto& list) {
            double value = std::stod(value_extractor(data));
            for (auto v : list)
                macro_values.insert_or_assign(macro_map_key{v, line}, value);
        };

        if (word_macros_map.count(data.word))
            macro_value_inserter(word_macros_map.find(data.word)->second);
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

private:
    macro_map& macro_values;
    int        line;
};

void SystemMacroAssigner::operator()(macro_map& macro_values, int line, const std::vector<AttributeVariant>& value)
{
    SystemMacroAssignerVisitor system_macro_assigner_visitor(macro_values, line);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(system_macro_assigner_visitor, value[x]);
}

} // namespace fanuc
} // namespace parser
