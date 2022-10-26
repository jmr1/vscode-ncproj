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

class RotationVerifierVisitor : public boost::static_visitor<>
{
public:
    void setContext(const CncDefaultValues& cnc_default_values, ELanguage language);

    void operator()(const DecimalAttributeData& data);

    template <typename T>
    void operator()(const T& data)
    {
    }

    bool was_S() const
    {
        return S;
    }

    bool was_G() const
    {
        return G;
    }

    bool was_M() const
    {
        return M;
    }

    void reset();

private:
    const CncDefaultValues* cnc_default_values;
    ELanguage               language;
    bool                    S{};
    bool                    M{};
    bool                    G{};
};

class PARSER_API RotationVerifier
{
public:
    void operator()(const CncDefaultValues& cnc_default_values, const std::vector<AttributeVariant>& value,
                    ELanguage language);

private:
    RotationVerifierVisitor rotation_verifier_visitor;
};

} // namespace fanuc
} // namespace parser
