#include "stdafx.h"

#include "CodeGroupsVerifier.h"

#include <string>

#include <boost/variant.hpp>

#include "MessageTextImpl.h"
#include "util.h"

namespace parser {
namespace fanuc {

class CodeGroupsVisitor : public boost::static_visitor<>
{
public:
    CodeGroupsVisitor(const std::string& code, const code_groups_map& codes, const macro_map& macro_values, int line,
                      ELanguage language, bool report_missing)
        : code(code)
        , codes_map(codes)
        , macro_values_map(macro_values)
        , line(line)
        , language(language)
        , report_missing(report_missing)
    {
    }

    using code_occurences_map = std::map<std::string, const CodeGroupValue*>;

    void operator()(const DecimalAttributeData& data)
    {
        if (data.word != code)
            return;
        CodeGroupValue cgv{};
        if (!data.value)
            return;
        unsigned int val = std::stoi(*data.value);
        if (data.macro)
        {
            auto macro_itor = macro_values_map.lower_bound({val, line});
            if (macro_itor == std::end(macro_values_map) ||
                (macro_itor != std::end(macro_values_map) && macro_itor->first.id != val))
                throw code_groups_exception(make_message(MessageName::MissingValue, language, "#" + *data.value));
            std::string macro_value(to_string_trunc(macro_itor->second));
            auto        pos = macro_value.find('.');
            if (pos != std::string::npos)
            {
                cgv.code = std::stoi(macro_value.substr(0, pos));
                cgv.rest = std::stoi(macro_value.substr(pos + 1));
            }
            else
            {
                cgv.code = std::stoi(macro_value);
            }
        }
        else
        {
            cgv.code = val;
            if (data.value2)
                cgv.rest = std::stoi(*data.value2);
        }

        auto itor = codes_map.find(cgv);
        if (itor == std::end(codes_map))
        {
            if (report_missing)
            {
                std::string tmp(code + std::to_string(cgv.code));
                if (cgv.rest != 0)
                    tmp += "." + std::to_string(cgv.rest);
                throw code_groups_exception(make_message2(MessageName::MissingInCodeGroups, language, tmp));
            }
            else
            {
                return;
            }
        }

        for (const auto& gr : itor->second)
        {
            code_occurences_map::iterator occ_itor;
            if ((occ_itor = occurences.find(gr)) != std::end(occurences))
            {
                std::string tmp(code + std::to_string(cgv.code));
                if (cgv.rest != 0)
                {
                    tmp += ".";
                    tmp += std::to_string(cgv.rest);
                }

                auto        cgv_occ = occ_itor->second;
                std::string tmp2    = code + std::to_string(cgv_occ->code);
                if (cgv_occ->rest != 0)
                {
                    tmp2 += ".";
                    tmp2 += std::to_string(cgv_occ->rest);
                }
                throw code_groups_exception(make_message2(MessageName::InSameCodeGroup, language, tmp, tmp2, gr));
            }

            occurences[gr] = &itor->first;
        }
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

private:
    const std::string      code;
    const code_groups_map& codes_map;
    const macro_map&       macro_values_map;
    code_occurences_map    occurences;
    const int              line{};
    const ELanguage        language;
    const bool             report_missing;
};

void CodeGroupsVerifier::operator()(const macro_map& macro_values, const code_groups_map& gcode_groups,
                                    const code_groups_map& mcode_groups, int line,
                                    const std::vector<AttributeVariant>& value, ELanguage language,
                                    bool report_missing /*= false*/)
{
    CodeGroupsVisitor gcode_groups_visitor("G", gcode_groups, macro_values, line, language, report_missing);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(gcode_groups_visitor, value[x]);
    CodeGroupsVisitor mcode_groups_visitor("M", mcode_groups, macro_values, line, language, report_missing);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(mcode_groups_visitor, value[x]);
}

} // namespace fanuc
} // namespace parser
