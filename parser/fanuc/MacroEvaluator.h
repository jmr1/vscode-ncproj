#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "MacroDefines.h"
#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

class MacroEvaluator
{
public:
    static bool evaluate(macro_map& macro_values, int line, const std::vector<AttributeVariant>& value,
                         std::string& message, bool single_line_msg, ELanguage language);
};

} // namespace fanuc
} // namespace parser
