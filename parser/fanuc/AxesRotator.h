#pragma once

#include "parser_export.h"

#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"

namespace parser {

enum class AxesRotatingOption;

namespace fanuc {

class PARSER_API AxesRotator
{
public:
    void operator()(AxesRotatingOption option, std::vector<AttributeVariant>& value);

private:
};

} // namespace fanuc
} // namespace parser
