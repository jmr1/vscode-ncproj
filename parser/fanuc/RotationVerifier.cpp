#include "stdafx.h"

#include "RotationVerifier.h"

#include "MessageTextImpl.h"
#include <boost/variant.hpp>

#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

void RotationVerifierVisitor::setContext(const CncDefaultValues& cnc_default_values, ELanguage language)
{
    this->cnc_default_values = &cnc_default_values;
    this->language           = language;
}

void RotationVerifierVisitor::reset()
{
    G = M = S = error_reported = false;
}

void RotationVerifierVisitor::operator()(const DecimalAttributeData& data)
{
    if (data.word == "S")
    {
        S = true;
        if (cnc_default_values->operator_turns_on_rotation)
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
            if (cnc_default_values->operator_turns_on_rotation)
            {
                throw rotation_verifier_exception(
                    make_message(MessageName::M3M4NotAllowedWhenOperatorTurnsRotation, language));
            }
        }
    }
    else if (!cnc_default_values->operator_turns_on_rotation && data.word == "G" && data.value)
    {
        int32_t value = std::stoi(*data.value);
        if (value == 1 || (value >= 81 && value <= 89))
        {
            G = true;
        }
    }
}

void RotationVerifier::operator()(const CncDefaultValues&              cnc_default_values,
                                  const std::vector<AttributeVariant>& value, ELanguage language)
{
    rotation_verifier_visitor.setContext(cnc_default_values, language);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(rotation_verifier_visitor, value[x]);
    if (!rotation_verifier_visitor.was_error_reported() && !cnc_default_values.operator_turns_on_rotation)
    {
        if (rotation_verifier_visitor.was_G() && !rotation_verifier_visitor.was_M() &&
            !rotation_verifier_visitor.was_S())
        {
            rotation_verifier_visitor.set_error_reported();
            throw rotation_verifier_exception(make_message(MessageName::ValueNotSet, language, std::string("S")));
        }
        if (rotation_verifier_visitor.was_G() && rotation_verifier_visitor.was_M() &&
            !rotation_verifier_visitor.was_S())
        {
            rotation_verifier_visitor.set_error_reported();
            throw rotation_verifier_exception(make_message(MessageName::ValueNotSet, language, std::string("S")));
        }
        if (rotation_verifier_visitor.was_G() && rotation_verifier_visitor.was_S() &&
            !rotation_verifier_visitor.was_M())
        {
            rotation_verifier_visitor.set_error_reported();
            throw rotation_verifier_exception(make_message(MessageName::ValueNotSet, language, std::string("M3/M4")));
        }
    }
}

} // namespace fanuc
} // namespace parser
