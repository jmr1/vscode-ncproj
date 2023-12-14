#pragma once

#include "parser_export.h"

#include <vector>

#include "AllAttributesParserDefines.h"
#include "MacroDefines.h"

namespace parser {
namespace fanuc {

class PARSER_API SystemMacroAssigner
{
public:
    void operator()(macro_map& macro_values, int line, const std::vector<AttributeVariant>& value);
};

} // namespace fanuc
} // namespace parser
