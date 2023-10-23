#include "stdafx.h"

#include "NCSettingsReader.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Logger.h"

namespace pt = boost::property_tree;

#define LOGGER (*mLogger)()

using namespace parser;

namespace nclangsrv {

NCSettingsReader::NCSettingsReader(const std::string& ncSettingsPath, Logger* logger)
    : mNcSettingsPath(ncSettingsPath)
    , mLogger(logger)
{
    setDefaults();
}

void NCSettingsReader::setDefaults()
{
    mMachineTool     = EMachineTool::mill_3axis;
    mMachineToolType = EMachineToolType::Mill;
    mFanucParserType = EFanucParserType::FanucMill;

    mMachinePointsData = MachinePointsData{{{"X", 0.0}, {"Y", 0.0}, {"Z", 500.0}},

                                           {{"X", 0.0}, {"Y", 0.0}, {"Z", 500.0}, {"I", 0.0}, {"J", 0.0}, {"K", 0.0}}};

    mKinematics = Kinematics{{{"X", {-500., 500.}}, {"Y", {-500., 500.}}, {"Z", {0., 1000.}}},
                             20000,
                             20000,
                             10000,
                             30,
                             6,
                             1,
                             3,
                             false,
                             false,
                             "",
                             "",
                             "",
                             ""};

    mCncDefaultValues = CncDefaultValues{EMotion::RapidTraverse, // G0
                                         EWorkPlane::XY /* G17 */,
                                         EDriverUnits::Millimeter,
                                         EProgrammingType::Absolute, // G90
                                         EFeedMode::PerMinute,
                                         ERotationDirection::Right,
                                         EDepthProgrammingType::Absolute,
                                         EDrillGreturnMode::G98,
                                         0,
                                         false,
                                         false,
                                         false,
                                         false,
                                         false,
                                         false,
                                         false,
                                         false};
}

bool NCSettingsReader::read()
{
    if (mRead)
        return true;

    try
    {
        pt::ptree root;
        pt::read_json(mNcSettingsPath, root);

        {
            mMachineTool     = root.get<EMachineTool>("machine_tool");
            mMachineToolType = root.get<EMachineToolType>("machine_tool_type");
            mFanucParserType = root.get<EFanucParserType>("driver");
        }

        {
            mMachinePointsData.tool_exchange_point.clear();
            mMachinePointsData.machine_base_point.clear();
            const auto& mpd = root.get_child("machine_points_data");
            for (const auto& [axis, data] : mpd.get_child("tool_exchange_point"))
                mMachinePointsData.tool_exchange_point.emplace(std::make_pair(
                    axis, data.get_value<decltype(MachinePointsData::tool_exchange_point)::mapped_type>()));
            for (const auto& [axis, data] : mpd.get_child("machine_base_point"))
                mMachinePointsData.machine_base_point.emplace(std::make_pair(
                    axis, data.get_value<decltype(MachinePointsData::machine_base_point)::mapped_type>()));
        }

        {
            mKinematics.cartesian_system_axis.clear();
            const auto& k_pt = root.get_child("kinematics");
            for (const auto& [axis, data] : k_pt.get_child("cartesian_system_axis"))
            {
                mKinematics.cartesian_system_axis.emplace(std::make_pair<const std::string&, AxisParameters>(
                    axis, {data.get<decltype(AxisParameters::range_min)>("range_min"),
                           data.get<decltype(AxisParameters::range_max)>("range_max")}));
            }
            auto& k                 = mKinematics;
            k.max_working_feed      = k_pt.get<decltype(k.max_working_feed)>("max_working_feed");
            k.max_fast_feed         = k_pt.get<decltype(k.max_fast_feed)>("max_fast_feed");
            k.maximum_spindle_speed = k_pt.get<decltype(k.maximum_spindle_speed)>("maximum_spindle_speed");
            k.numer_of_items_in_the_warehouse =
                k_pt.get<decltype(k.numer_of_items_in_the_warehouse)>("numer_of_items_in_the_warehouse");
            k.tool_exchange_time      = k_pt.get<decltype(k.tool_exchange_time)>("tool_exchange_time");
            k.pallet_exchange_time    = k_pt.get<decltype(k.pallet_exchange_time)>("pallet_exchange_time");
            k.tool_measurement_time   = k_pt.get<decltype(k.tool_measurement_time)>("tool_measurement_time");
            k.diameter_programming_2x = k_pt.get<decltype(k.diameter_programming_2x)>("diameter_programming_2x");
            k.auto_measure_after_tool_selection =
                k_pt.get<decltype(k.auto_measure_after_tool_selection)>("auto_measure_after_tool_selection");
            k.pallet_exchange_code = k_pt.get<decltype(k.pallet_exchange_code)>("pallet_exchange_code");
            k.pallet_exchange_code_value =
                k_pt.get<decltype(k.pallet_exchange_code_value)>("pallet_exchange_code_value");
            k.tool_measurement_code = k_pt.get<decltype(k.tool_measurement_code)>("tool_measurement_code");
            k.tool_measurement_code_value =
                k_pt.get<decltype(k.tool_measurement_code_value)>("tool_measurement_code_value");
        }

        {
            const auto& cnc        = root.get_child("cnc_default_values");
            auto&       c          = mCncDefaultValues;
            c.default_motion       = cnc.get<decltype(c.default_motion)>("default_motion");
            c.default_work_plane   = cnc.get<decltype(c.default_work_plane)>("default_work_plane");
            c.default_driver_units = cnc.get<decltype(c.default_driver_units)>("default_driver_units");
            c.default_programming  = cnc.get<decltype(c.default_programming)>("default_programming");
            c.default_feed_mode    = cnc.get<decltype(c.default_feed_mode)>("default_feed_mode");
            c.default_rotation_direction =
                cnc.get<decltype(c.default_rotation_direction)>("default_rotation_direction");
            c.drill_cycle_z_value      = cnc.get<decltype(c.drill_cycle_z_value)>("drill_cycle_z_value");
            c.drill_cycle_return_value = cnc.get<decltype(c.drill_cycle_return_value)>("drill_cycle_return_value");
            c.default_rotation         = cnc.get<decltype(c.default_rotation)>("default_rotation");
            c.rapid_traverse_cancel_cycle =
                cnc.get<decltype(c.rapid_traverse_cancel_cycle)>("rapid_traverse_cancel_cycle");
            c.linear_interpolation_cancel_cycle =
                cnc.get<decltype(c.linear_interpolation_cancel_cycle)>("linear_interpolation_cancel_cycle");
            c.circular_interpolation_cw_cancel_cycle =
                cnc.get<decltype(c.circular_interpolation_cw_cancel_cycle)>("circular_interpolation_cw_cancel_cycle");
            c.circular_interpolation_ccw_cancel_cycle =
                cnc.get<decltype(c.circular_interpolation_ccw_cancel_cycle)>("circular_interpolation_ccw_cancel_cycle");
            c.cycle_cancel_starts_rapid_traverse =
                cnc.get<decltype(c.cycle_cancel_starts_rapid_traverse)>("cycle_cancel_starts_rapid_traverse");
            c.operator_turns_on_rotation =
                cnc.get<decltype(c.operator_turns_on_rotation)>("operator_turns_on_rotation");
            c.tool_number_executes_exchange =
                cnc.get<decltype(c.tool_number_executes_exchange)>("tool_number_executes_exchange");
            c.auto_rapid_traverse_after_tool_exchange =
                cnc.get<decltype(c.auto_rapid_traverse_after_tool_exchange)>("auto_rapid_traverse_after_tool_exchange");
        }

        {
            const auto& zp = root.get_child("zero_point");
            mZeroPoint.x   = zp.get<decltype(ZeroPoint::x)>("X");
            mZeroPoint.y   = zp.get<decltype(ZeroPoint::y)>("Y");
            mZeroPoint.z   = zp.get<decltype(ZeroPoint::z)>("Z");
        }
    }
    catch (const pt::json_parser_error& e)
    {
        if (mLogger)
        {
            LOGGER << "NCSettingsReader::" << __func__ << ": ERR: " << e.what() << std::endl;
            mLogger->flush();
        }
        return false;
    }
    catch (const std::exception& e)
    {
        if (mLogger)
        {
            LOGGER << "NCSettingsReader::" << __func__ << ": ERR: " << e.what() << std::endl;
            mLogger->flush();
        }
        return false;
    }

    mRead = true;

    return true;
}

} // namespace nclangsrv