#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "CodeGroupsDefines.h"
#include "GeneralParserDefines.h"
#include "MacroDefines.h"

namespace parser {
namespace fanuc {

struct path_calc_exception : std::runtime_error
{
    explicit path_calc_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

enum class EHelixType
{
    None    = 0,
    Full    = 360,
    Quarter = 90,
};

class AttributesPathCalculator
{
public:
    AttributesPathCalculator(EMachineToolType machine_tool_type, MachinePointsData&& machine_points_data,
                             Kinematics&& kinematics, CncDefaultValues&& cnc_default_values, ELanguage language);
    AttributesPathCalculator(EMachineToolType machine_tool_type, const MachinePointsData& machine_points_data,
                             const Kinematics& kinematics, const CncDefaultValues& cnc_default_values,
                             ELanguage language);

    void reset();

    void evaluate(const std::vector<AttributeVariant>& value, EDriverUnits units);

    const PathResult& get_path_result() const
    {
        return path_result;
    }

    const TimeResult& get_time_result() const
    {
        return time_result;
    }

    const word_range_map& get_word_range() const
    {
        return word_range;
    }

    const Kinematics& get_kinematics() const
    {
        return kinematics;
    }

    const CncDefaultValues& get_cnc_default_values() const
    {
        return cnc_default_values;
    }

private:
    const EMachineToolType        machine_tool_type;
    const MachinePointsData       machine_points_data;
    Kinematics                    kinematics;
    CncDefaultValues              cnc_default_values;
    ELanguage                     language;
    PathResult                    path_result{};
    TimeResult                    time_result{};
    std::map<std::string, double> prev_values;
    std::map<std::string, double> t_path;
    std::map<std::string, double> t_time;
    word_range_map                word_range;
    double                        active_f{-1};
    double                        active_s{-1};
    int                           active_g{-1};
    EWorkPlane                    active_g171819{};
    EDrillGreturnMode             active_g_return{};
    EProgrammingType              active_g9091{};
    EConstantSurfaceSpeedControl  active_g9697{};
    EFeedMode                     active_feed_mode{};
    EHelixType                    active_helix{};
    std::string                   active_t;
    bool                          active_auto_rapid_traverse_after_tool_exchange{};
};

} // namespace fanuc
} // namespace parser
