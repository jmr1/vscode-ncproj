#include "stdafx.h"

#include "RangeVerifier.h"

#include "MessageTextImpl.h"
#include <boost/variant.hpp>

#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

class RangeVerifierVisitor : public boost::static_visitor<>
{
public:
    RangeVerifierVisitor(EMachineToolType machine_tool_type, const Kinematics& kinematics, ELanguage language)
        : machine_tool_type(machine_tool_type)
        , kinematics(kinematics)
        , language(language)
    {
    }

    void operator()(const DecimalAttributeData& data)
    {
        if (data.word == "S")
        {
            if (data.value && std::stoi(*data.value) > kinematics.maximum_spindle_speed)
                throw range_verifier_exception(make_message(MessageName::ValueHigherThanAllowed, language,
                                                            std::string("S"),
                                                            *data.value,
                                                            std::to_string(kinematics.maximum_spindle_speed)));
        }
        else if (data.word == "T")
        {
            if (data.value)
            {
                auto tool_number{*data.value};
                if (machine_tool_type == EMachineToolType::Lathe && tool_number.size() > 2)
                    tool_number = tool_number.substr(0, tool_number.size() / 2);

                if (std::stoi(tool_number) > kinematics.numer_of_items_in_the_warehouse)
                    throw range_verifier_exception(
                        make_message(MessageName::ValueHigherThanAllowed, language, std::string("T"), tool_number,
                                     std::to_string(kinematics.numer_of_items_in_the_warehouse)));
            }
        }
        else if (data.word == "F")
        {
            if (data.value && std::stoi(*data.value) > kinematics.max_working_feed)
                throw range_verifier_exception(make_message(MessageName::ValueHigherThanAllowed, language,
                                                            std::string("F"),
                                                            *data.value, std::to_string(kinematics.max_working_feed)));
        }
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

private:
    EMachineToolType  machine_tool_type;
    const Kinematics& kinematics;
    const ELanguage   language;
};

void RangeVerifier::operator()(EMachineToolType machine_tool_type, const Kinematics& kinematics,
                               const std::vector<AttributeVariant>& value, ELanguage language)
{
    RangeVerifierVisitor range_verifier_visitor(machine_tool_type, kinematics, language);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(range_verifier_visitor, value[x]);
}

void RangeVerifier::operator()(const Kinematics& kinematics, const ZeroPoint& zero_point, const word_range_map& wrm, ELanguage language)
{
    for (const auto& it : wrm)
    {
        const auto& itr = kinematics.cartesian_system_axis.find(it.first);
        if (itr == kinematics.cartesian_system_axis.end())
            continue;

        double current{};
        std::tie(std::ignore, current, std::ignore) = it.second;
        auto range_min                              = itr->second.range_min;
        auto range_max                              = itr->second.range_max;

        if (itr->first == "X")
        {
            range_min -= zero_point.x;
            range_max -= zero_point.x;
        }
        else if (itr->first == "Y")
        {
            range_min -= zero_point.y;
            range_max -= zero_point.y;
        }
        else if (itr->first == "Z")
        {
            range_min -= zero_point.z;
            range_max -= zero_point.z;
        }

        if (current > range_max || current < range_min)
            throw range_verifier_exception(make_message(MessageName::ValueOutOfRange, language, it.first,
                                                        std::to_string(current),
                                                        std::to_string(range_min), std::to_string(range_max)));
    }
}

} // namespace fanuc
} // namespace parser
