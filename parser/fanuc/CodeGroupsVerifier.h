#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "CodeGroupsDefines.h"
#include "GeneralParserDefines.h"
#include "MacroDefines.h"

namespace parser {
namespace fanuc {

struct code_groups_exception : std::runtime_error
{
    explicit code_groups_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

class CodeGroupsVerifier
{
public:
    void operator()(const macro_map& macro_values, const code_groups_map& gcode_groups,
                    const code_groups_map& mcode_groups, int line, const std::vector<AttributeVariant>& value,
                    ELanguage language);
};

} // namespace fanuc
} // namespace parser
