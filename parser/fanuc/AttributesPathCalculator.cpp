#include "stdafx.h"

#include "AttributesPathCalculator.h"

#include <optional>
#include <string>
#include <vector>

#include <boost/variant.hpp>

#include "MessageTextImpl.h"
#include "PathCalculator.h"

namespace parser {
namespace fanuc {

const std::vector<EDrillGmode> drill_cycle = {EDrillGmode::G74, EDrillGmode::G81, EDrillGmode::G82, EDrillGmode::G84,
                                              EDrillGmode::G85, EDrillGmode::G86, EDrillGmode::G88, EDrillGmode::G89};
const std::vector<EDrillGmode> drill_cycle_qparam = {EDrillGmode::G73, EDrillGmode::G76, EDrillGmode::G83,
                                                     EDrillGmode::G87};

bool is_drill_cycle(int value)
{
    return std::any_of(std::begin(drill_cycle), std::end(drill_cycle),
                       [value](EDrillGmode gmode) { return static_cast<int>(gmode) == value; });
}
bool is_drill_cycle_qparam(int value)
{
    return std::any_of(std::begin(drill_cycle_qparam), std::end(drill_cycle_qparam),
                       [value](EDrillGmode gmode) { return static_cast<int>(gmode) == value; });
}

class AttributesPathCalculatorVisitor : public boost::static_visitor<>
{
public:
    AttributesPathCalculatorVisitor(EMachineToolType machine_tool_type, const Kinematics& kinematics)
        : machine_tool_type(machine_tool_type)
        , kinematics(kinematics)
    {
        for (const auto& it : kinematics.cartesian_system_axis)
            gcode_params.insert(it.first);
    }

    void operator()(const DecimalAttributeData& data)
    {
        if (data.word == "/")
        {
            was_comment = true;
            return;
        }

        if (was_comment)
            return;

        auto itor = std::find_if(std::cbegin(gcode_params), std::cend(gcode_params),
                                 [&](const std::string& value) { return data.word == value; });

        auto itor_other = std::find_if(std::cbegin(other_code_params), std::cend(other_code_params),
                                       [&](const std::string& value) { return data.word == value; });

        if (itor == std::end(gcode_params) && itor_other == std::end(other_code_params))
            return;

        if (itor != std::end(gcode_params))
        {
            if (!data.value && !data.dot)
                return;
            std::string tmp;
            // if(data.assign)
            //    tmp += *data.assign;
            if (data.sign)
                tmp += *data.sign;
            if (data.value)
                tmp += *data.value;
            if (data.dot)
                tmp += *data.dot;
            if (data.value2)
                tmp += *data.value2;
            values[data.word] = std::stod(tmp);
            if (data.word == "P" && !data.dot)
                values[data.word] /= 1000;
        }
        else if (data.word == "T")
        {
            if (!data.value)
                return;
            if (data.dot || data.value2)
                return;
            auto tool_number{*data.value};
            if (machine_tool_type == EMachineToolType::Lathe && tool_number.size() > 2)
                tool_number = tool_number.substr(0, tool_number.size() / 2);
            active_t.push_back(tool_number);
        }
        else if (data.word == "F")
        {
            if (!data.value && !data.dot)
                return;
            std::string tmp;
            if (data.value)
                tmp = *data.value;
            if (data.dot)
            {
                tmp += *data.dot;
                if (data.value2)
                    tmp += *data.value2;
            }
            active_f = std::stod(tmp);
        }
        else if (data.word == "S")
        {
            if (!data.value && !data.dot)
                return;
            std::string tmp;
            if (data.value)
                tmp = *data.value;
            if (data.dot)
            {
                tmp += *data.dot;
                if (data.value2)
                    tmp += *data.value2;
            }
            active_s = std::stod(tmp);
        }
        else if (data.word == "M")
        {
            if (!data.value)
                return;
            if (data.dot || data.value2)
                return;
            if (std::stoi(*data.value) == 6)
                m6 = true;
            if (data.word == kinematics.pallet_exchange_code && *data.value == kinematics.pallet_exchange_code_value)
                pallet_exchange = true;
            if (!kinematics.auto_measure_after_tool_selection)
            {
                if (data.word == kinematics.tool_measurement_code &&
                    *data.value == kinematics.tool_measurement_code_value)
                    tool_measurement = true;
            }
        }
        else // G
        {
            if (!data.value)
                return;
            const auto val = std::stoi(*data.value);
            if (const auto vale = static_cast<EWorkPlane>(val);
                vale == EWorkPlane::XY || vale == EWorkPlane::XZ || vale == EWorkPlane::YZ)
                active_g171819 = vale;
            else if ((val >= 0 && val <= 3) || val == 80 || is_drill_cycle(val) || is_drill_cycle_qparam(val))
                active_g = val;
            else if (const auto vale = static_cast<EFeedMode>(val);
                     vale == EFeedMode::PerMinute || vale == EFeedMode::PerRevolution)
                active_feed_mode = vale;
            else if (const auto vale = static_cast<EConstantSurfaceSpeedControl>(val);
                     vale == EConstantSurfaceSpeedControl::G96 || vale == EConstantSurfaceSpeedControl::G97)
                active_g9697 = vale;
            else if (const auto vale = static_cast<EDrillGreturnMode>(val);
                     vale == EDrillGreturnMode::G98 || vale == EDrillGreturnMode::G99)
                active_g_return = vale;
            else if (const auto vale = static_cast<EProgrammingType>(val);
                     vale == EProgrammingType::Absolute || vale == EProgrammingType::Incremental)
                active_g9091 = vale;
            else
            {
                auto tmp = *data.value;
                if (data.dot && data.value2)
                    tmp += *data.dot + *data.value2;
                other_g.push_back(tmp);
            }
        }
    }

    void operator()(const StringAttributeData& data)
    {
        if (data.word == kinematics.pallet_exchange_code && data.value == kinematics.pallet_exchange_code_value)
            pallet_exchange = true;
        if (!kinematics.auto_measure_after_tool_selection)
        {
            if (data.word == kinematics.tool_measurement_code && data.value == kinematics.tool_measurement_code_value)
                tool_measurement = true;
        }
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

    const std::map<std::string, double>& get_values() const
    {
        return values;
    }

    auto get_f() const
    {
        return active_f;
    }

    auto get_s() const
    {
        return active_s;
    }

    auto get_g() const
    {
        return active_g;
    }

    auto get_g171819() const
    {
        return active_g171819;
    }

    auto get_g_return() const
    {
        return active_g_return;
    }

    auto get_g9091() const
    {
        return active_g9091;
    }

    auto get_g9697() const
    {
        return active_g9697;
    }

    auto get_feed_mode() const
    {
        return active_feed_mode;
    }

    const std::vector<std::string>& get_other_g() const
    {
        return other_g;
    }

    auto was_m6() const
    {
        return m6;
    }

    std::optional<std::string> get_t() const
    {
        if (active_t.empty())
            return std::nullopt;
        else
        {
            if (m6)
                return std::make_optional(active_t.front());
            else
                return std::make_optional(active_t.back());
        }
    }

    auto was_pallet_exchange() const
    {
        return pallet_exchange;
    }

    auto was_tool_measurement() const
    {
        return tool_measurement;
    }

private:
    const EMachineToolType                      machine_tool_type;
    const Kinematics&                           kinematics;
    std::set<std::string>                       gcode_params = {"X", "Y", "Z", "I", "J", "K", "R", "Q", "L", "P", "U"};
    const std::vector<std::string>              other_code_params = {"G", "F", "M", "T", "S"};
    std::map<std::string, double>               values;
    std::optional<double>                       active_f;
    std::optional<double>                       active_s;
    std::optional<int>                          active_g;
    std::optional<EWorkPlane>                   active_g171819;
    std::optional<EDrillGreturnMode>            active_g_return;
    std::optional<EProgrammingType>             active_g9091;
    std::optional<EConstantSurfaceSpeedControl> active_g9697;
    std::optional<EFeedMode>                    active_feed_mode;
    std::vector<std::string>                    other_g;
    std::vector<std::string>                    active_t;
    bool                                        m6{};
    bool                                        was_comment{};
    bool                                        pallet_exchange{};
    bool                                        tool_measurement{};
}; // namespace fanuc

struct copy_on_return
{
    copy_on_return(const int& active_g, const EProgrammingType& active_g9091,
                   const std::map<std::string, double>& values, std::map<std::string, double>& prev_values,
                   word_range_map& word_range, const std::vector<std::string>& other_g = {})
        : v(values)
        , pv(prev_values)
        , wr(word_range)
        , og(other_g)
        , g(active_g)
        , g9091(active_g9091)
    {
    }

    ~copy_on_return()
    {
        for (const auto& it : v)
        {
            if (it.first == "Z" && (is_drill_cycle(g) || is_drill_cycle_qparam(g)))
                continue;
            if (it.first == "Q" || it.first == "L")
                continue;

            if (!og.empty())
            {
                if (const auto& axis = ignore_g_codes.find(it.first); axis != std::end(ignore_g_codes))
                {
                    if (std::any_of(std::begin(axis->second), std::end(axis->second), [&](const auto& axis_value) {
                            return std::any_of(std::begin(og), std::end(og),
                                               [&](const auto& og_value) { return og_value == axis_value; });
                        }))
                        continue;
                }
            }

            if (g9091 == EProgrammingType::Incremental && (it.first == "X" || it.first == "Y" || it.first == "Z"))
            {
                if (auto writ = wr.find(it.first); writ != std::end(wr))
                {
                    auto& [min, current, max] = writ->second;
                    current += it.second;
                    if (current < min)
                        min = current;
                    else if (current > max)
                        max = current;
                    pv[it.first] = current;
                }
                else
                {
                    wr[it.first] = std::make_tuple(it.second, it.second, it.second);
                    pv[it.first] = it.second;
                }
            }
            else
            {
                pv[it.first] = it.second;
                if (auto writ = wr.find(it.first); writ != std::end(wr))
                {
                    auto& [min, current, max] = writ->second;
                    current                   = it.second;
                    if (it.second < min)
                        min = it.second;
                    else if (it.second > max)
                        max = it.second;
                }
                else
                {
                    wr[it.first] = std::make_tuple(it.second, it.second, it.second);
                }
            }
        }
    }

private:
    const std::map<std::string, std::vector<std::string>> ignore_g_codes = {
        {"X", {"4", "10", "50.1", "51.1", "52", "53", "68", "68.2", "68.3", "68.4", "92"}},
        {"Y", {"10", "50.1", "51.1", "52", "53", "68", "68.2", "68.3", "68.4", "92"}},
        {"Z", {"10", "50.1", "51.1", "52", "53", "68", "68.2", "68.3", "68.4", "92"}},
        {"I", {"68.2", "68.4"}},
        {"J", {"68.2", "68.4"}},
        {"K", {"68.2", "68.4"}},
        {"R", {"5", "5.1", "6.2", "10", "68", "68.3"}},
        {"U", {"71", "72"}},
        {"W", {"71", "72"}},
    };
    const std::map<std::string, double>& v;
    std::map<std::string, double>&       pv;
    word_range_map&                      wr;
    const std::vector<std::string>&      og;
    const int&                           g;
    const EProgrammingType&              g9091;
};

AttributesPathCalculator::AttributesPathCalculator(EMachineTool machine_tool, EMachineToolType machine_tool_type,
                                                   MachinePointsData&& machine_points_data, Kinematics&& kinematics,
                                                   CncDefaultValues&& cnc_default_values, ELanguage language)
    : machine_tool(machine_tool)
    , machine_tool_type(machine_tool_type)
    , machine_points_data(std::move(machine_points_data))
    , kinematics(std::move(kinematics))
    , cnc_default_values(std::move(cnc_default_values))
    , language(language)
    , active_s(this->cnc_default_values.operator_turns_on_rotation ? this->cnc_default_values.default_rotation : -1)
    , active_g(static_cast<int>(cnc_default_values.default_motion))
    , active_g171819(this->cnc_default_values.default_work_plane)
    , active_g_return(this->cnc_default_values.drill_cycle_return_value)
    , active_g9091(this->cnc_default_values.default_programming)
    , active_feed_mode(this->cnc_default_values.default_feed_mode)
{
    copy_on_return cr{active_g, active_g9091, machine_points_data.machine_base_point, prev_values, word_range};
}

AttributesPathCalculator::AttributesPathCalculator(EMachineTool machine_tool, EMachineToolType machine_tool_type,
                                                   const MachinePointsData& machine_points_data,
                                                   const Kinematics&        kinematics,
                                                   const CncDefaultValues& cnc_default_values, ELanguage language)
    : machine_tool(machine_tool)
    , machine_tool_type(machine_tool_type)
    , machine_points_data(machine_points_data)
    , kinematics(kinematics)
    , cnc_default_values(cnc_default_values)
    , language(language)
    , active_s(this->cnc_default_values.operator_turns_on_rotation ? this->cnc_default_values.default_rotation : -1)
    , active_g(static_cast<int>(cnc_default_values.default_motion))
    , active_g171819(this->cnc_default_values.default_work_plane)
    , active_g_return(this->cnc_default_values.drill_cycle_return_value)
    , active_g9091(this->cnc_default_values.default_programming)
    , active_feed_mode(this->cnc_default_values.default_feed_mode)
{
    copy_on_return cr{active_g, active_g9091, machine_points_data.machine_base_point, prev_values, word_range};
}

void AttributesPathCalculator::reset()
{
    path_result = PathResult{}; // clear
    time_result = TimeResult{}; // clear
    word_range.clear();
    t_path.clear();
    t_time.clear();
    prev_values.clear();
    active_f         = -1;
    active_s         = cnc_default_values.operator_turns_on_rotation ? cnc_default_values.default_rotation : -1;
    active_g         = static_cast<int>(cnc_default_values.default_motion);
    active_g171819   = cnc_default_values.default_work_plane;
    active_g_return  = cnc_default_values.drill_cycle_return_value;
    active_g9091     = cnc_default_values.default_programming;
    active_g9697     = {};
    active_feed_mode = cnc_default_values.default_feed_mode;
    active_helix     = EHelixType::None;
    active_t.clear();
    copy_on_return cr{active_g, active_g9091, machine_points_data.machine_base_point, prev_values, word_range};
}

template <typename T, typename U>
std::optional<U> get_optional_value(const std::map<T, U>& values, const std::string& param)
{
    if (const auto it = values.find(param); it != std::end(values))
        return it->second;
    return std::nullopt;
}

void AttributesPathCalculator::evaluate(const std::vector<AttributeVariant>& value, EDriverUnits units)
{
    path_result.work_motion = 0;
    path_result.fast_motion = 0;
    time_result.work_motion = 0;
    time_result.fast_motion = 0;

    if (value.size() == 0)
        return;

    AttributesPathCalculatorVisitor attributes_visitor(machine_tool_type, kinematics);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(attributes_visitor, value[x]);

    const auto  t                    = attributes_visitor.get_t();
    const auto  f                    = attributes_visitor.get_f();
    const auto  s                    = attributes_visitor.get_s();
    const auto  g                    = attributes_visitor.get_g();
    const auto  g171819              = attributes_visitor.get_g171819();
    const auto  g_return             = attributes_visitor.get_g_return();
    const auto  g9091                = attributes_visitor.get_g9091();
    const auto  g9697                = attributes_visitor.get_g9697();
    const auto  feed_mode            = attributes_visitor.get_feed_mode();
    const auto& other_g              = attributes_visitor.get_other_g();
    const auto& values               = attributes_visitor.get_values();
    const auto  was_m6               = attributes_visitor.was_m6();
    const auto  was_pallet_exchange  = attributes_visitor.was_pallet_exchange();
    const auto  was_tool_measurement = attributes_visitor.was_tool_measurement();

    bool move_to_tool_exchange_point{};
    bool move_to_machine_base_point{};

    if (!cnc_default_values.tool_number_executes_exchange && was_m6)
    {
        move_to_tool_exchange_point = true;
        time_result.total += kinematics.tool_exchange_time;
        if (kinematics.auto_measure_after_tool_selection)
            time_result.total += kinematics.tool_measurement_time;
    }
    if (!kinematics.auto_measure_after_tool_selection && was_tool_measurement)
        time_result.total += kinematics.tool_measurement_time;
    if (was_pallet_exchange)
        time_result.total += kinematics.pallet_exchange_time;

    if (t)
    {
        if (cnc_default_values.auto_rapid_traverse_after_tool_exchange)
            active_auto_rapid_traverse_after_tool_exchange = true;

        if (!active_t.empty())
        {
            t_path[active_t] = path_result.tool_total;
            t_time[active_t] = time_result.tool_total;
        }

        if (cnc_default_values.tool_number_executes_exchange ||
            (!cnc_default_values.tool_number_executes_exchange && was_m6))
            active_t = *t;

        path_result.tool_id = time_result.tool_id = active_t;

        if (cnc_default_values.tool_number_executes_exchange)
        {
            move_to_tool_exchange_point = true;
            time_result.total += kinematics.tool_exchange_time;
            if (kinematics.auto_measure_after_tool_selection)
                time_result.total += kinematics.tool_measurement_time;
        }

        path_result.tool_total = 0;
        if (const auto it = t_path.find(active_t); it != std::end(t_path))
            path_result.tool_total = it->second;

        time_result.tool_total = 0;
        if (const auto it = t_time.find(active_t); it != std::end(t_time))
            time_result.tool_total = it->second;

        if (!move_to_tool_exchange_point)
        {
            if (g || g171819 || g_return || g9091)
            {
                int v{};
                if (g)
                    v = *g;
                else if (g171819)
                    v = static_cast<int>(*g171819);
                else if (g_return)
                    v = static_cast<int>(*g_return);
                else if (g9091)
                    v = static_cast<int>(*g9091);
                else if (g9697)
                    v = static_cast<int>(*g9697);
                throw path_calc_exception(make_message(MessageName::TNotWithG, language, std::to_string(v)));
            }

            copy_on_return cr{
                active_g,   active_g9091, values, prev_values,
                word_range, other_g}; // save axis in case the start values are provided together with tool change
            return;
        }
    }

    if (std::find_if(std::begin(other_g), std::end(other_g),
                     [](const std::string& val) { return val == "04" || val == "4"; }) != std::end(other_g))
    {
        if (const auto xit = values.find("X"); xit != std::cend(values))
            time_result.total += xit->second;
        if (const auto xit = values.find("U"); xit != std::cend(values))
            time_result.total += xit->second;
        if (const auto xit = values.find("P"); xit != std::cend(values))
            time_result.total += xit->second;
    }

    if (std::find_if(std::begin(other_g), std::end(other_g), [](const std::string& val) { return val == "28"; }) !=
        std::end(other_g))
        move_to_machine_base_point = true;

    if (move_to_tool_exchange_point || move_to_machine_base_point)
        active_g = 0;

    if (active_auto_rapid_traverse_after_tool_exchange && !values.empty())
    {
        active_auto_rapid_traverse_after_tool_exchange = false;
        active_g                                       = 0;
    }
    else if (!move_to_tool_exchange_point && !move_to_machine_base_point)
    {
        if (active_g == -1 && !g)
        {
            if (g171819)
                active_g171819 = *g171819;
            if (g9091)
                active_g9091 = *g9091;
            if (g9697)
                active_g9697 = *g9697;
            if (g_return)
                active_g_return = *g_return;
            copy_on_return cr{
                active_g,   active_g9091, values, prev_values,
                word_range, other_g}; // save axis because here may be start values before any G code is being used
            return;
        }

        // if we have active_g or g (and if we are here we do because previous if statement already checked that) then
        // we should continue calculations
        /*if(!g && !other_g.empty())
        {
            if(g171819)
                active_g171819 = *g171819;
            if(g9091)
                active_g9091 = *g9091;
            if(g9697)
                active_g9697 = *g9697;
            if(g_return)
                active_g_return = *g_return;
            copy_on_return cr{active_g, active_g9091, values, prev_values, word_range, other_g}; // save axis because
        here may be start values together with other irrelevant G codes return; // skip other G codes and their axis
        values
        }*/
    }

    auto current_g = g ? *g : active_g;
    auto current_f = f ? *f : active_f;
    auto current_s = s ? *s : active_s;

    const auto current_g171819   = g171819 ? *g171819 : active_g171819;
    const auto current_g9091     = g9091 ? *g9091 : active_g9091;
    const auto current_g9697     = g9697 ? *g9697 : active_g9697;
    const auto current_g_return  = g_return ? *g_return : active_g_return;
    const auto current_feed_mode = feed_mode ? *feed_mode : active_feed_mode;

    const auto lathe_programming =
        machine_tool_type == EMachineToolType::Lathe ||
        (machine_tool_type == EMachineToolType::Millturn && current_g171819 == EWorkPlane::XZ);
    const auto lathe_non_diameter = lathe_programming && !kinematics.diameter_programming_2x;

    if (current_g == 0 || current_g == 1)
    {
        // if(move_to_tool_exchange_point)
        //    if(!g && values.size() == 0) // seems nothing useful here
        //        return;

        if ((current_g == 0 && cnc_default_values.rapid_traverse_cancel_cycle) ||
            (current_g == 1 && cnc_default_values.linear_interpolation_cancel_cycle))
        {
            path_result.cycle_distance = {};
            path_result.hole_number    = 0;
        }

        const auto xit_prev = prev_values.find("X");
        const auto yit_prev = prev_values.find("Y");
        const auto zit_prev = prev_values.find("Z");

        if (current_g9091 == EProgrammingType::Absolute || move_to_tool_exchange_point || move_to_machine_base_point)
        {
            if (xit_prev == std::end(prev_values))
                throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, "X"));
            if (yit_prev == std::end(prev_values))
                throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, "Y"));
            if (zit_prev == std::end(prev_values))
                throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, "Z"));

            const auto xit = !move_to_tool_exchange_point && !move_to_machine_base_point
                                 ? values.find("X")
                                 : move_to_tool_exchange_point ? machine_points_data.tool_exchange_point.find("X")
                                                               : machine_points_data.machine_base_point.find("X");
            const auto yit = !move_to_tool_exchange_point && !move_to_machine_base_point
                                 ? values.find("Y")
                                 : move_to_tool_exchange_point ? machine_points_data.tool_exchange_point.find("Y")
                                                               : machine_points_data.machine_base_point.find("Y");
            const auto zit = !move_to_tool_exchange_point && !move_to_machine_base_point
                                 ? values.find("Z")
                                 : move_to_tool_exchange_point ? machine_points_data.tool_exchange_point.find("Z")
                                                               : machine_points_data.machine_base_point.find("Z");

            const auto x_curr =
                ((!move_to_tool_exchange_point && !move_to_machine_base_point && xit != std::cend(values)) ||
                 (move_to_tool_exchange_point && xit != std::cend(machine_points_data.tool_exchange_point)) ||
                 (move_to_machine_base_point && xit != std::cend(machine_points_data.machine_base_point)))
                    ? xit->second
                    : xit_prev->second;
            const auto y_curr =
                ((!move_to_tool_exchange_point && !move_to_machine_base_point && yit != std::cend(values)) ||
                 (move_to_tool_exchange_point && yit != std::cend(machine_points_data.tool_exchange_point)) ||
                 (move_to_machine_base_point && yit != std::cend(machine_points_data.machine_base_point)))
                    ? yit->second
                    : yit_prev->second;
            const auto z_curr =
                ((!move_to_tool_exchange_point && !move_to_machine_base_point && zit != std::cend(values)) ||
                 (move_to_tool_exchange_point && zit != std::cend(machine_points_data.tool_exchange_point)) ||
                 (move_to_machine_base_point && zit != std::cend(machine_points_data.machine_base_point)))
                    ? zit->second
                    : zit_prev->second;

            const auto xprv = lathe_non_diameter ? xit_prev->second / 2 : xit_prev->second;
            const auto xcur = lathe_non_diameter ? x_curr / 2 : x_curr;
            if (current_g == 0)
                path_result.fast_motion =
                    PathCalculator::G0(xprv, yit_prev->second, zit_prev->second, xcur, y_curr, z_curr);
            else
                path_result.work_motion =
                    PathCalculator::G1(xprv, yit_prev->second, zit_prev->second, xcur, y_curr, z_curr);
        }
        else
        {
            const auto xit = values.find("X");
            const auto yit = values.find("Y");
            const auto zit = values.find("Z");

            if (xit == std::cend(values) && xit_prev == std::end(prev_values))
                throw path_calc_exception(make_message(MessageName::MissingValue, language, "X"));
            if (yit == std::end(values) && yit_prev == std::end(prev_values))
                throw path_calc_exception(make_message(MessageName::MissingValue, language, "Y"));
            if (zit == std::end(values) && zit_prev == std::end(prev_values))
                throw path_calc_exception(make_message(MessageName::MissingValue, language, "Z"));

            const auto x_curr = xit != std::cend(values) ? xit->second : 0.;
            const auto y_curr = yit != std::cend(values) ? yit->second : 0.;
            const auto z_curr = zit != std::cend(values) ? zit->second : 0.;

            const auto xcur = lathe_non_diameter ? x_curr / 2 : x_curr;
            if (current_g == 0)
                path_result.fast_motion = PathCalculator::G0(xcur, y_curr, z_curr);
            else
                path_result.work_motion = PathCalculator::G1(xcur, y_curr, z_curr);
        }
    }
    else if (current_g == 2 || current_g == 3)
    {
        if (!g && values.size() == 0) // seems nothing useful here
            return;

        if ((current_g == 2 && cnc_default_values.circular_interpolation_cw_cancel_cycle) ||
            (current_g == 3 && cnc_default_values.circular_interpolation_ccw_cancel_cycle))
        {
            path_result.cycle_distance = {};
            path_result.hole_number    = 0;
        }

        std::string axis1, axis2, axis3, param1, param2, param3;
        using fn_6dbl                   = double(double, double, double, double, double, double);
        using fn_5dbl                   = double(double, double, double, double, double);
        using fn_4dbl                   = double(double, double, double, double);
        using fn_3dbl                   = double(double, double, double);
        using fn_2dbl                   = double(double, double);
        using fn_g                      = fn_6dbl;
        using fn_g_incr                 = fn_4dbl;
        using fn_g_r                    = fn_5dbl;
        using fn_g_r_incr               = fn_3dbl;
        using fn_g_helix                = fn_4dbl;
        using fn_g_helix_incr           = fn_3dbl;
        using fn_g_helix_r              = fn_3dbl;
        using fn_g_helix_r_incr         = fn_2dbl;
        using fn_g_helix_quarter        = fn_4dbl;
        using fn_g_helix_quarter_incr   = fn_3dbl;
        using fn_g_helix_quarter_r      = fn_3dbl;
        using fn_g_helix_quarter_r_incr = fn_2dbl;
        using fn_g_helix_archimedes_q   = fn_3dbl;
        using fn_g_helix_archimedes_l   = fn_3dbl;
        std::function<fn_g>                      gfunc;
        std::function<fn_g_incr>                 gfunc_incr;
        std::function<fn_g_r>                    gfunc_r;
        std::function<fn_g_r_incr>               gfunc_r_incr;
        std::function<fn_g_helix>                gfunc_helix;
        std::function<fn_g_helix_incr>           gfunc_helix_incr;
        std::function<fn_g_helix_r>              gfunc_helix_r;
        std::function<fn_g_helix_r_incr>         gfunc_helix_r_incr;
        std::function<fn_g_helix_quarter>        gfunc_helix_quarter;
        std::function<fn_g_helix_quarter_incr>   gfunc_helix_quarter_incr;
        std::function<fn_g_helix_quarter_r>      gfunc_helix_quarter_r;
        std::function<fn_g_helix_quarter_r_incr> gfunc_helix_quarter_r_incr;
        std::function<fn_g_helix_archimedes_q>   gfunc_helix_archimedes_q;
        std::function<fn_g_helix_archimedes_l>   gfunc_helix_archimedes_l;

        if (current_g171819 == EWorkPlane::XY)
        {
            axis1  = "X";
            axis2  = "Y";
            axis3  = "Z";
            param1 = "I";
            param2 = "J";
            param3 = "K";
            if (current_g == 2)
            {
                gfunc      = static_cast<fn_g*>(&PathCalculator::G2_G17);
                gfunc_incr = static_cast<fn_g_incr*>(&PathCalculator::G2_G17);
            }
            else
            {
                gfunc      = static_cast<fn_g*>(&PathCalculator::G3_G17);
                gfunc_incr = static_cast<fn_g_incr*>(&PathCalculator::G3_G17);
            }
            gfunc_r                  = static_cast<fn_g_r*>(&PathCalculator::G2G3_G17);
            gfunc_r_incr             = static_cast<fn_g_r_incr*>(&PathCalculator::G2G3_G17);
            gfunc_helix              = static_cast<fn_g_helix*>(&PathCalculator::G2G3_HelixG17);
            gfunc_helix_incr         = static_cast<fn_g_helix_incr*>(&PathCalculator::G2G3_HelixG17);
            gfunc_helix_r            = static_cast<fn_g_helix_r*>(&PathCalculator::G2G3_HelixG17R);
            gfunc_helix_r_incr       = static_cast<fn_g_helix_r_incr*>(&PathCalculator::G2G3_HelixG17R);
            gfunc_helix_quarter      = static_cast<fn_g_helix_quarter*>(&PathCalculator::G2G3_HelixG17_Quarter);
            gfunc_helix_quarter_incr = static_cast<fn_g_helix_quarter_incr*>(&PathCalculator::G2G3_HelixG17_Quarter);
            gfunc_helix_quarter_r    = static_cast<fn_g_helix_quarter_r*>(&PathCalculator::G2G3_HelixG17_QuarterR);
            gfunc_helix_quarter_r_incr =
                static_cast<fn_g_helix_quarter_r_incr*>(&PathCalculator::G2G3_HelixG17_QuarterR);
            gfunc_helix_archimedes_q =
                static_cast<fn_g_helix_archimedes_q*>(&PathCalculator::G2G3_HelixG17_ArchimedesQparam);
            gfunc_helix_archimedes_l =
                static_cast<fn_g_helix_archimedes_l*>(&PathCalculator::G2G3_HelixG17_ArchimedesLparam);
        }
        else if (current_g171819 == EWorkPlane::XZ)
        {
            axis1  = "X";
            axis2  = "Z";
            axis3  = "Y";
            param1 = "I";
            param2 = "K";
            param3 = "J";
            if (current_g == 2)
            {
                gfunc      = static_cast<fn_g*>(&PathCalculator::G2_G18);
                gfunc_incr = static_cast<fn_g_incr*>(&PathCalculator::G2_G18);
            }
            else
            {
                gfunc      = static_cast<fn_g*>(&PathCalculator::G3_G18);
                gfunc_incr = static_cast<fn_g_incr*>(&PathCalculator::G3_G18);
            }
            gfunc_r                  = static_cast<fn_g_r*>(&PathCalculator::G2G3_G18);
            gfunc_r_incr             = static_cast<fn_g_r_incr*>(&PathCalculator::G2G3_G18);
            gfunc_helix              = static_cast<fn_g_helix*>(&PathCalculator::G2G3_HelixG18);
            gfunc_helix_incr         = static_cast<fn_g_helix_incr*>(&PathCalculator::G2G3_HelixG18);
            gfunc_helix_r            = static_cast<fn_g_helix_r*>(&PathCalculator::G2G3_HelixG18R);
            gfunc_helix_r_incr       = static_cast<fn_g_helix_r_incr*>(&PathCalculator::G2G3_HelixG18R);
            gfunc_helix_quarter      = static_cast<fn_g_helix_quarter*>(&PathCalculator::G2G3_HelixG18_Quarter);
            gfunc_helix_quarter_incr = static_cast<fn_g_helix_quarter_incr*>(&PathCalculator::G2G3_HelixG18_Quarter);
            gfunc_helix_quarter_r    = static_cast<fn_g_helix_quarter_r*>(&PathCalculator::G2G3_HelixG18_QuarterR);
            gfunc_helix_quarter_r_incr =
                static_cast<fn_g_helix_quarter_r_incr*>(&PathCalculator::G2G3_HelixG18_QuarterR);
            gfunc_helix_archimedes_q =
                static_cast<fn_g_helix_archimedes_q*>(&PathCalculator::G2G3_HelixG18_ArchimedesQparam);
            gfunc_helix_archimedes_l =
                static_cast<fn_g_helix_archimedes_l*>(&PathCalculator::G2G3_HelixG18_ArchimedesLparam);
        }
        else
        {
            axis1  = "Y";
            axis2  = "Z";
            axis3  = "X";
            param1 = "J";
            param2 = "K";
            param3 = "I";
            if (current_g == 2)
            {
                gfunc      = static_cast<fn_g*>(&PathCalculator::G2_G19);
                gfunc_incr = static_cast<fn_g_incr*>(&PathCalculator::G2_G19);
            }
            else
            {
                gfunc      = static_cast<fn_g*>(&PathCalculator::G3_G19);
                gfunc_incr = static_cast<fn_g_incr*>(&PathCalculator::G3_G19);
            }
            gfunc_r                  = static_cast<fn_g_r*>(&PathCalculator::G2G3_G19);
            gfunc_r_incr             = static_cast<fn_g_r_incr*>(&PathCalculator::G2G3_G19);
            gfunc_helix              = static_cast<fn_g_helix*>(&PathCalculator::G2G3_HelixG19);
            gfunc_helix_incr         = static_cast<fn_g_helix_incr*>(&PathCalculator::G2G3_HelixG19);
            gfunc_helix_r            = static_cast<fn_g_helix_r*>(&PathCalculator::G2G3_HelixG19R);
            gfunc_helix_r_incr       = static_cast<fn_g_helix_r_incr*>(&PathCalculator::G2G3_HelixG19R);
            gfunc_helix_quarter      = static_cast<fn_g_helix_quarter*>(&PathCalculator::G2G3_HelixG19_Quarter);
            gfunc_helix_quarter_incr = static_cast<fn_g_helix_quarter_incr*>(&PathCalculator::G2G3_HelixG19_Quarter);
            gfunc_helix_quarter_r    = static_cast<fn_g_helix_quarter_r*>(&PathCalculator::G2G3_HelixG19_QuarterR);
            gfunc_helix_quarter_r_incr =
                static_cast<fn_g_helix_quarter_r_incr*>(&PathCalculator::G2G3_HelixG19_QuarterR);
            gfunc_helix_archimedes_q =
                static_cast<fn_g_helix_archimedes_q*>(&PathCalculator::G2G3_HelixG19_ArchimedesQparam);
            gfunc_helix_archimedes_l =
                static_cast<fn_g_helix_archimedes_l*>(&PathCalculator::G2G3_HelixG19_ArchimedesLparam);
        }

        const auto x_prev = get_optional_value(prev_values, axis1);
        const auto y_prev = get_optional_value(prev_values, axis2);
        const auto z_prev = get_optional_value(prev_values, axis3); // will be checked later in helix
        if (!x_prev)
            throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, axis1));
        if (!y_prev)
            throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, axis2));

        auto i = get_optional_value(values, param1);
        auto j = get_optional_value(values, param2);
        auto k = get_optional_value(values, param3);
        auto r = get_optional_value(values, "R");
        auto q = get_optional_value(values, "Q");
        auto l = get_optional_value(values, "L");

        const auto x = get_optional_value(values, axis1);
        const auto y = get_optional_value(values, axis2);
        const auto z = get_optional_value(values, axis3);

        bool calculate_arc                  = true;
        bool first_helix_quarter_incr_block = false;

        if (q || l)
        {
            calculate_arc = false;

            if (!i)
                i = get_optional_value(prev_values, param1);
            if (!j)
                j = get_optional_value(prev_values, param2);

            if (!i)
                throw path_calc_exception(make_message(MessageName::MissingValue, language, param1));
            if (!j)
                throw path_calc_exception(make_message(MessageName::MissingValue, language, param2));

            if (q)
                path_result.work_motion = gfunc_helix_archimedes_q(*i, *j, *q);
            else
                path_result.work_motion = gfunc_helix_archimedes_l(*i, *j, *l);
        }
        else if (active_helix == EHelixType::None && x && y && z && ((i && j && k) || r))
        {
            if (!z_prev)
                throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, axis3));
            if ((*x == *x_prev && *y == *y_prev) || (*x == 0. && *y == 0.))
            {
                active_helix = EHelixType::Full;
            }
            else
            {
                active_helix                   = EHelixType::Quarter;
                first_helix_quarter_incr_block = true;
            }
        }

        if (active_helix != EHelixType::None)
        {
            calculate_arc     = false;
            const auto i_prev = get_optional_value(prev_values, param1);
            const auto j_prev = get_optional_value(prev_values, param2);
            const auto k_prev = get_optional_value(prev_values, param3);
            const auto r_prev = get_optional_value(prev_values, "R");

            if ((!r && !r_prev) && ((!i && !i_prev) || (!j && !j_prev)))
                throw path_calc_exception(
                    make_message(MessageName::ValueNotSetForEitherEither, language, param1, param2, param3));

            if (!i)
                i = i_prev;

            if (!j)
                j = j_prev;

            if (!k && k_prev)
                k = k_prev;

            if (!r)
                r = r_prev;

            if (!r && (!i || !j || !k))
                throw path_calc_exception(
                    make_message(MessageName::ValueNotSetForEitherEither, language, param1, param2, param3));

            if (current_g9091 == EProgrammingType::Incremental)
            {
                if (active_helix == EHelixType::Full)
                {
                    if ((!x || *x == 0.) && (!y || *y == 0.))
                    {
                        if (i && j)
                            path_result.work_motion = gfunc_helix_incr(*z, *i, *j);
                        else
                            path_result.work_motion = gfunc_helix_r_incr(*z, *r);
                    }
                    else
                    {
                        if (i && j)
                            path_result.work_motion = PathCalculator::G2G3_HelixLast(gfunc_helix_incr(*z, *i, *j));
                        else
                            path_result.work_motion = PathCalculator::G2G3_HelixLast(gfunc_helix_r_incr(*z, *r));
                        active_helix = EHelixType::None;
                    }
                }
                else if (active_helix == EHelixType::Quarter)
                {
                    if (first_helix_quarter_incr_block || (!first_helix_quarter_incr_block && x && i && j))
                    {
                        first_helix_quarter_incr_block = false;
                        if (i && j)
                            path_result.work_motion = gfunc_helix_quarter_incr(*z, *i, *j);
                        else
                            path_result.work_motion = gfunc_helix_quarter_r_incr(*z, *r);
                    }
                    else
                    {
                        active_helix  = EHelixType::None;
                        calculate_arc = true;
                    }
                }
            }
            else
            {
                const auto x_curr = x ? *x : *x_prev;
                const auto y_curr = y ? *y : *y_prev;
                const auto z_curr = z ? *z : *z_prev;
                if (active_helix == EHelixType::Full)
                {
                    if (*x_prev == x_curr && *y_prev == y_curr)
                    {
                        if (i && j)
                            path_result.work_motion = gfunc_helix(*z_prev, *z, *i, *j);
                        else
                            path_result.work_motion = gfunc_helix_r(*z_prev, *z, *r);
                    }
                    else
                    {
                        if (i && j)
                            path_result.work_motion = PathCalculator::G2G3_HelixLast(gfunc_helix(*z_prev, *z, *i, *j));
                        else
                            path_result.work_motion = PathCalculator::G2G3_HelixLast(gfunc_helix_r(*z_prev, *z, *r));
                        active_helix = EHelixType::None;
                    }
                }
                else if (active_helix == EHelixType::Quarter)
                {
                    if (*x_prev != x_curr && *y_prev != y_curr && *z_prev != z_curr)
                    {
                        if (i && j)
                            path_result.work_motion = gfunc_helix_quarter(*z_prev, *z, *i, *j);
                        else
                            path_result.work_motion = gfunc_helix_quarter_r(*z_prev, *z, *r);
                    }
                    else
                    {
                        active_helix  = EHelixType::None;
                        calculate_arc = true;
                    }
                }
            }
        }

        if (calculate_arc && active_helix == EHelixType::None)
        {
            if (!r && (!i || !j))
            {
                if (!i)
                    i = get_optional_value(prev_values, param1);

                if (!j)
                    j = get_optional_value(prev_values, param2);
            }

            if (!r && (!i || !j))
                throw path_calc_exception(make_message(MessageName::ValueNotSetForEither, language, param1, param2));

            if (current_g9091 == EProgrammingType::Incremental)
            {
                const auto x_curr = x ? *x_prev + *x : *x_prev;
                const auto xprv   = lathe_non_diameter ? *x_prev / 2 : *x_prev;
                const auto xcur   = lathe_non_diameter ? x_curr / 2 : x_curr;

                if (i && j)
                {
                    const auto i_curr = lathe_non_diameter ? *i / 2 : *i;
                    // path_result.work_motion = gfunc_incr(x ? *x : 0., y ? *y : 0., *i, *j);
                    path_result.work_motion = gfunc(xprv, *y_prev, xcur, y ? *y_prev + *y : *y_prev, i_curr, *j);
                }
                else
                {
                    // path_result.work_motion = gfunc_r_incr(x ? *x : 0., y ? *y : 0., *r);
                    path_result.work_motion = gfunc_r(xprv, *y_prev, xcur, y ? *y_prev + *y : *y_prev, *r);
                }
            }
            else
            {
                const auto x_curr = x ? *x : *x_prev;
                const auto y_curr = y ? *y : *y_prev;
                const auto xprv   = lathe_non_diameter ? *x_prev / 2 : *x_prev;
                const auto xcur   = lathe_non_diameter ? x_curr / 2 : x_curr;

                if (i && j)
                {
                    const auto i_curr       = lathe_non_diameter ? *i / 2 : *i;
                    path_result.work_motion = gfunc(xprv, *y_prev, xcur, y_curr, i_curr, *j);
                }
                else
                {
                    path_result.work_motion = gfunc_r(xprv, *y_prev, xcur, y_curr, *r);
                }
            }
        }
    }
    else if (current_g == 80)
    {
        path_result.cycle_distance = {};
        path_result.hole_number    = 0;
        if (cnc_default_values.cycle_cancel_starts_rapid_traverse)
            current_g = 0; // G0
    }
    else if (bool dc = false, dcq = false; (dc = is_drill_cycle(current_g)) || (dcq = is_drill_cycle_qparam(current_g)))
    {
        if (!g && values.size() == 0) // seems nothing useful here
            return;

        std::string axis1, axis2, axis3, axis_gleb;
        using fn_drill_cycle =
            std::pair<double, double>(double, double, double, EDepthProgrammingType, EDrillGmode, EDrillGreturnMode);
        using fn_drill_cycle_incr   = std::pair<double, double>(double, double, EDrillGmode, EDrillGreturnMode);
        using fn_drill_cycle_qparam = std::pair<double, double>(double, double, double, double, EDepthProgrammingType,
                                                                EDrillGmode, EDrillGreturnMode);
        using fn_drill_cycle_qparam_incr =
            std::pair<double, double>(double, double, double, EDrillGmode, EDrillGreturnMode);
        using fn_drill_cycle_hole =
            std::pair<double, double>(double, double, double, double, const std::pair<double, double>&);
        using fn_drill_cycle_hole_incr = std::pair<double, double>(double, double, const std::pair<double, double>&);
        std::function<fn_drill_cycle>             func_drill_cycle;
        std::function<fn_drill_cycle_incr>        func_drill_cycle_incr;
        std::function<fn_drill_cycle_qparam>      func_drill_cycle_qparam;
        std::function<fn_drill_cycle_qparam_incr> func_drill_cycle_qparam_incr;
        std::function<fn_drill_cycle_hole>        func_drill_cycle_hole;
        std::function<fn_drill_cycle_hole_incr>   func_drill_cycle_hole_incr;

        if (current_g171819 == EWorkPlane::XY)
        {
            axis1                   = "X";
            axis2                   = "Y";
            axis3                   = "Z";
            axis_gleb               = "Z";
            func_drill_cycle        = static_cast<fn_drill_cycle*>(&PathCalculator::DrillCyclePath_G17);
            func_drill_cycle_incr   = static_cast<fn_drill_cycle_incr*>(&PathCalculator::DrillCyclePath_G17);
            func_drill_cycle_qparam = static_cast<fn_drill_cycle_qparam*>(&PathCalculator::DrillCyclePath_G17);
            func_drill_cycle_qparam_incr =
                static_cast<fn_drill_cycle_qparam_incr*>(&PathCalculator::DrillCyclePath_G17);
            func_drill_cycle_hole = static_cast<fn_drill_cycle_hole*>(&PathCalculator::DrillCyclePathHole_G17);
            func_drill_cycle_hole_incr =
                static_cast<fn_drill_cycle_hole_incr*>(&PathCalculator::DrillCyclePathHole_G17);
        }
        else if (current_g171819 == EWorkPlane::XZ)
        {
            axis1                   = "X";
            axis2                   = "Z";
            axis3                   = "Y";
            axis_gleb               = "Y";
            func_drill_cycle        = static_cast<fn_drill_cycle*>(&PathCalculator::DrillCyclePath_G18);
            func_drill_cycle_incr   = static_cast<fn_drill_cycle_incr*>(&PathCalculator::DrillCyclePath_G18);
            func_drill_cycle_qparam = static_cast<fn_drill_cycle_qparam*>(&PathCalculator::DrillCyclePath_G18);
            func_drill_cycle_qparam_incr =
                static_cast<fn_drill_cycle_qparam_incr*>(&PathCalculator::DrillCyclePath_G18);
            func_drill_cycle_hole = static_cast<fn_drill_cycle_hole*>(&PathCalculator::DrillCyclePathHole_G18);
            func_drill_cycle_hole_incr =
                static_cast<fn_drill_cycle_hole_incr*>(&PathCalculator::DrillCyclePathHole_G18);
        }
        else
        {
            axis1                   = "Y";
            axis2                   = "Z";
            axis3                   = "X";
            axis_gleb               = "X";
            func_drill_cycle        = static_cast<fn_drill_cycle*>(&PathCalculator::DrillCyclePath_G19);
            func_drill_cycle_incr   = static_cast<fn_drill_cycle_incr*>(&PathCalculator::DrillCyclePath_G19);
            func_drill_cycle_qparam = static_cast<fn_drill_cycle_qparam*>(&PathCalculator::DrillCyclePath_G19);
            func_drill_cycle_qparam_incr =
                static_cast<fn_drill_cycle_qparam_incr*>(&PathCalculator::DrillCyclePath_G19);
            func_drill_cycle_hole = static_cast<fn_drill_cycle_hole*>(&PathCalculator::DrillCyclePathHole_G19);
            func_drill_cycle_hole_incr =
                static_cast<fn_drill_cycle_hole_incr*>(&PathCalculator::DrillCyclePathHole_G19);
        }

        const auto k = get_optional_value(values, "K");
        if (path_result.hole_number == 0 || k) // we want to calculate this even though *k=0 or k is with G90 because
                                               // the results may be needed in next blocks
        {
            const bool zgleb_is_lathe_without_Y =
                machine_tool == EMachineTool::lathe_zx ||
                machine_tool == EMachineTool::lathe_zxc; // TODO: change this to check if axis is in axes machine
                                                         // configuration (nc_settings.json: axes array)
            const auto zit_prev = prev_values.find(axis3);
            if (zit_prev == std::end(prev_values))
                throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, axis3));
            const auto zglebit = values.find(axis_gleb);
            if (zglebit == std::end(values) && !zgleb_is_lathe_without_Y)
                throw path_calc_exception(make_message(MessageName::MissingValue, language, axis_gleb + "gleb"));
            const auto rit = values.find("R");
            if (rit == std::end(values))
                throw path_calc_exception(make_message(MessageName::MissingValue, language, "R"));

            std::pair<double, double> ret;
            if (dcq)
            {
                const auto qit = values.find("Q");
                if (rit == std::end(values))
                    throw path_calc_exception(make_message(MessageName::MissingValue, language, "Q"));
                if (current_g9091 == EProgrammingType::Incremental)
                {
                    ret = func_drill_cycle_qparam_incr(!zgleb_is_lathe_without_Y ? zglebit->second : 0.0, rit->second,
                                                       qit->second, static_cast<EDrillGmode>(current_g),
                                                       current_g_return);
                }
                else
                {
                    ret = func_drill_cycle_qparam(zit_prev->second, !zgleb_is_lathe_without_Y ? zglebit->second : 0.0,
                                                  rit->second, qit->second, cnc_default_values.drill_cycle_z_value,
                                                  static_cast<EDrillGmode>(current_g), current_g_return);
                }
            }
            else
            {
                if (current_g9091 == EProgrammingType::Incremental)
                {
                    ret = func_drill_cycle_incr(!zgleb_is_lathe_without_Y ? zglebit->second : 0.0, rit->second,
                                                static_cast<EDrillGmode>(current_g), current_g_return);
                }
                else
                {
                    ret = func_drill_cycle(zit_prev->second, !zgleb_is_lathe_without_Y ? zglebit->second : 0.0,
                                           rit->second, cnc_default_values.drill_cycle_z_value,
                                           static_cast<EDrillGmode>(current_g), current_g_return);
                }
            }
            path_result.cycle_distance = ret;
        }

        const auto x_prev = get_optional_value(prev_values, axis1);
        const auto y_prev = get_optional_value(prev_values, axis2);
        if (!x_prev)
            throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, axis1));
        if (!y_prev)
            throw path_calc_exception(make_message(MessageName::MissingPreviousValue, language, axis2));

        const auto x = get_optional_value(values, axis1);
        const auto y = get_optional_value(values, axis2);

        if (k && *k == 0) // handle special case for both G90 and G91
        {
            decltype(path_result.cycle_distance) cycle_distance_tmp{};
            if (current_g9091 == EProgrammingType::Incremental)
            {
                std::tie(path_result.work_motion, path_result.fast_motion) =
                    func_drill_cycle_hole_incr(x ? *x : 0., y ? *y : 0., cycle_distance_tmp);
            }
            else
            {
                const auto x_curr = x ? *x : *x_prev;
                const auto y_curr = y ? *y : *y_prev;
                std::tie(path_result.work_motion, path_result.fast_motion) =
                    func_drill_cycle_hole(*x_prev, *y_prev, x_curr, y_curr, cycle_distance_tmp);
            }
        }
        else
        {
            if (current_g9091 == EProgrammingType::Incremental)
            {
                int holes = 1;
                if (k && k > 0)
                    holes = static_cast<decltype(holes)>(*k);
                for (int h = 0; h < holes; ++h)
                {
                    auto [work_motion, fast_motion] =
                        func_drill_cycle_hole_incr(x ? *x : 0., y ? *y : 0., path_result.cycle_distance);
                    path_result.work_motion += work_motion;
                    path_result.fast_motion += fast_motion;
                }
            }
            else
            {
                const auto x_curr = x ? *x : *x_prev;
                const auto y_curr = y ? *y : *y_prev;
                std::tie(path_result.work_motion, path_result.fast_motion) =
                    func_drill_cycle_hole(*x_prev, *y_prev, x_curr, y_curr, path_result.cycle_distance);
            }
        }
        ++path_result.hole_number;
    }
    else
    {
        return; // don't save anything
    }

    active_f         = current_f;
    active_s         = current_s;
    active_g         = current_g;
    active_g171819   = current_g171819;
    active_g_return  = current_g_return;
    active_g9091     = current_g9091;
    active_g9697     = current_g9697;
    active_feed_mode = current_feed_mode;

    path_result.tool_id = active_t;
    path_result.tool_total += path_result.work_motion + path_result.fast_motion;
    path_result.total += path_result.work_motion + path_result.fast_motion;

    time_result.tool_id = active_t;
    // here work_motion (and fast_motion) was set previously to 0.0 and is guaranteed to return true because 0.0 is
    // represented exactly as 0.0 in double active_f (and fast_feed) is set to -1.0 for unset value so we can check if
    // is non negative number to know if it was set
    if (path_result.work_motion != 0.0 && active_f > 0.0)
    {
        if (lathe_programming &&
            current_feed_mode == EFeedMode::PerRevolution /*&& current_g_return == EDrillGreturnMode::G99*/)
        {
            if (current_g9697 == EConstantSurfaceSpeedControl::G96)
            {
                double rpm{};
                if (units == EDriverUnits::Inch)
                    rpm = MathUtils::CalcTimeCSSinch(active_s, path_result.work_motion);
                else // Metric
                    rpm = MathUtils::CalcTimeCSSmm(active_s, path_result.work_motion);
                const auto feed_minutes = MathUtils::CalcFeedMinutes(active_f, rpm);
                time_result.work_motion = 60. * MathUtils::CalcTime(path_result.work_motion, feed_minutes);
            }
            else // G97
            {
                const auto feed_minutes = MathUtils::CalcFeedMinutes(active_f, active_s);
                time_result.work_motion = 60. * MathUtils::CalcTime(path_result.work_motion, feed_minutes);
            }
        }
        else // non lathe and lathe feed mode per minute
        {
            time_result.work_motion = 60. * MathUtils::CalcTime(path_result.work_motion, active_f);
        }
    }
    if (path_result.fast_motion != 0.0 && kinematics.max_fast_feed > 0.0)
        time_result.fast_motion = 60. * MathUtils::CalcTime(path_result.fast_motion, kinematics.max_fast_feed);
    time_result.tool_total += time_result.work_motion + time_result.fast_motion;
    time_result.total += time_result.work_motion + time_result.fast_motion;

    if (path_result.work_motion != 0.0 && active_f < 0.0)
        throw path_calc_exception(make_message(MessageName::ValueNotSet, language, "F"));
    if (path_result.fast_motion != 0.0 && kinematics.max_fast_feed < 0.0)
        throw path_calc_exception(make_message(MessageName::ValueNotSet, language, "Fast Feed"));

    if (path_result.work_motion != 0.0 && active_f > 0.0)
        if (lathe_programming && current_feed_mode == EFeedMode::PerRevolution)
            if (active_s < 0.0)
                throw path_calc_exception(make_message(MessageName::ValueNotSet, language, "S"));

    if (move_to_tool_exchange_point)
        copy_on_return cr{
            active_g, EProgrammingType::Absolute, machine_points_data.tool_exchange_point, prev_values, word_range,
            other_g};
    else if (move_to_machine_base_point)
        copy_on_return cr{
            active_g, EProgrammingType::Absolute, machine_points_data.machine_base_point, prev_values, word_range,
            other_g};
    else
        copy_on_return cr{active_g, active_g9091, values, prev_values, word_range, other_g};
}

} // namespace fanuc
} // namespace parser
