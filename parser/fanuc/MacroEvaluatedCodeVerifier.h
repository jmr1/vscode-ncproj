#pragma once

#include "parser_export.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "GeneralParserDefines.h"
#include "MacroDefines.h"

namespace parser {
namespace fanuc {

struct macro_evaluated_code_exception : std::runtime_error
{
    explicit macro_evaluated_code_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

class PARSER_API MacroEvaluatedCodeVerifier
{
public:
    std::string operator()(const std::vector<std::string>& allowed_operations, const macro_map& macro_values, int line,
                           const std::vector<AttributeVariant>& value, ELanguage language);
};

} // namespace fanuc
} // namespace parser
