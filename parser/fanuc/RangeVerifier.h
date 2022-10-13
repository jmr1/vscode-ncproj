#pragma once

#include "parser_export.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "GeneralParserDefines.h"

namespace parser {

struct Kinematics;
struct ZeroPoint;

namespace fanuc {

struct range_verifier_exception : std::runtime_error
{
    explicit range_verifier_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

class PARSER_API RangeVerifier
{
public:
    void operator()(EMachineToolType machine_tool_type, const Kinematics& kinematics,
                    const std::vector<AttributeVariant>& value, ELanguage language);
    void operator()(const Kinematics& kinematics, const ZeroPoint& zero_point, const word_range_map& wrm,
                    ELanguage language);
};

} // namespace fanuc
} // namespace parser
