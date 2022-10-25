#pragma once

#include "parser_export.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "GeneralParserDefines.h"

namespace parser {

struct CncDefaultValues;

namespace fanuc {

struct rotation_verifier_exception : std::runtime_error
{
    explicit rotation_verifier_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

class PARSER_API RotationVerifier
{
public:
    void operator()(const CncDefaultValues& cnc_default_values, const std::vector<AttributeVariant>& value,
                    ELanguage language);
};

} // namespace fanuc
} // namespace parser
