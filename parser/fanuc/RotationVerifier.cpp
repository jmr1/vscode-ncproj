#include "stdafx.h"

#include "RotationVerifier.h"

#include "MessageTextImpl.h"
#include <boost/variant.hpp>

#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

class RotationVerifierVisitor : public boost::static_visitor<>
{
public:
    RotationVerifierVisitor(const CncDefaultValues& cnc_default_values, ELanguage language)
        : cnc_default_values(cnc_default_values)
        , language(language)
    {
    }

    void operator()(const DecimalAttributeData& data)
    {
        if (data.word == "S")
        {
            S = true;
            if (cnc_default_values.operator_turns_on_rotation)
            {
                throw rotation_verifier_exception(
                    make_message(MessageName::SNotAllowedWhenOperatorTurnsRotation, language));
            }
        }
        else if (data.word == "M" && data.value)
        {
            int32_t value = std::stoi(*data.value);
            if (value == 3 || value == 4)
            {
                M = true;
                if (cnc_default_values.operator_turns_on_rotation)
                {
                    throw rotation_verifier_exception(
                        make_message(MessageName::M3M4NotAllowedWhenOperatorTurnsRotation, language));
                }
            }
        }
        else if (!cnc_default_values.operator_turns_on_rotation && data.word == "G" && data.value)
        {
            int32_t value = std::stoi(*data.value);
            if (value == 1 || (value >= 81 && value <= 89))
            {
                G = true;
            }
        }
    }

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

private:
    const CncDefaultValues& cnc_default_values;
    const ELanguage         language;
    bool                    S{};
    bool                    M{};
    bool                    G{};
};

void RotationVerifier::operator()(const CncDefaultValues&              cnc_default_values,
                                  const std::vector<AttributeVariant>& value, ELanguage language)
{
    RotationVerifierVisitor rotation_verifier_visitor(cnc_default_values, language);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(rotation_verifier_visitor, value[x]);
    if (!cnc_default_values.operator_turns_on_rotation)
    {
        if (rotation_verifier_visitor.was_G() && rotation_verifier_visitor.was_M() &&
            !rotation_verifier_visitor.was_S())
        {
            throw rotation_verifier_exception(make_message(MessageName::ValueMissing, language, std::string("S")));
        }
    }
}

} // namespace fanuc
} // namespace parser
