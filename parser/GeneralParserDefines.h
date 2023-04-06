#pragma once

#include "parser_export.h"

#include <map>
#include <string>
#include <vector>

#include "Macro.h"

namespace parser {

class AttributeVariantData
{
};

enum class ECncType
{
    Fanuc,
    Generic,
    Haas,
    Heidenhain,
    Makino,
};

LEFT_STREAM_OP_DECL(ECncType)
RIGHT_STREAM_OP_DECL(ECncType)

enum class EFanucParserType
{
    FanucLatheSystemA,
    FanucLatheSystemB,
    FanucLatheSystemC,
    FanucMill,
    FanucMillturnSystemA,
    FanucMillturnSystemB,
    GenericLathe,
    GenericMill,
    HaasLathe,
    HaasMill,
    MakinoMill,
};

LEFT_STREAM_OP_DECL(EFanucParserType)
RIGHT_STREAM_OP_DECL(EFanucParserType)

enum class EMachineToolType
{
    Mill,
    Lathe,
    Millturn,
};

LEFT_STREAM_OP_DECL(EMachineToolType)
RIGHT_STREAM_OP_DECL(EMachineToolType)

enum class EMachineTool
{
    mill_3axis,
    mill_4axis,
    mill_4axis_horizontal,
    mill_4axis_horizontal_pinola,
    mill_4axis_head_gantry,
    mill_5axis_head_head_gantry,
    mill_5axis_head_table_gantry,
    mill_5axis_table_table,
    mill_5axis_table_tilt,
    mill_5axis_head_table,
    mill_5axis_horizontal_table_table,
    mill_5axis_horizontal_table_table_pinola,
    lathe_zx,
    lathe_zxc,
    lathe_zxcy,
    lathe_zxcy_steady,
    millturn_XYZBC,
    millturn_XYZBC_steady,
    millturn_XYZBC_C2,
    millturn_XYZBC_C2_steady,
    millturn_XYZBC_C2_Lower_Left_Turret,
    millturn_XYZBC_C2_Lower_Right_Turret,
    millturn_XYZBC_C2_Lower_LeftRight_Turret,
};

LEFT_STREAM_OP_DECL(EMachineTool)
RIGHT_STREAM_OP_DECL(EMachineTool)

struct ParserSettings
{
    bool evaluate_macro;
    bool verify_code_groups;
    bool calculate_path;
    bool ncsettings_code_analysis;
    bool zero_point_analysis;
};

enum class ELanguage
{
    English,
    Polish,
};

LEFT_STREAM_OP_DECL(ELanguage)
RIGHT_STREAM_OP_DECL(ELanguage)

struct OtherSettings
{
    ELanguage language;
};

struct MachinePointsData
{
    std::map<std::string, double> tool_exchange_point;
    std::map<std::string, double> machine_base_point;
};

struct AxisParameters
{
    double range_min;
    double range_max;
};

struct Kinematics
{
    std::map<std::string, AxisParameters> cartesian_system_axis;
    double                                max_working_feed;
    double                                max_fast_feed;
    int                                   maximum_spindle_speed;
    int                                   numer_of_items_in_the_warehouse;
    int                                   tool_exchange_time;
    int                                   pallet_exchange_time;
    int                                   tool_measurement_time;
    bool                                  diameter_programming_2x;
    bool                                  auto_measure_after_tool_selection;
    std::string                           pallet_exchange_code;
    std::string                           pallet_exchange_code_value;
    std::string                           tool_measurement_code;
    std::string                           tool_measurement_code_value;
};

struct ZeroPoint
{
    double x;
    double y;
    double z;
};

enum class EMotion
{
    RapidTraverse,      // G0
    LinearInterpolation // G1
};

LEFT_STREAM_OP_DECL(EMotion)
RIGHT_STREAM_OP_DECL(EMotion)

enum class EWorkPlane
{
    XY = 17,
    XZ,
    YZ
};

LEFT_STREAM_OP_DECL(EWorkPlane)
RIGHT_STREAM_OP_DECL(EWorkPlane)

enum class EDriverUnits
{
    Millimeter,
    Inch
};

LEFT_STREAM_OP_DECL(EDriverUnits)
RIGHT_STREAM_OP_DECL(EDriverUnits)

enum class EFeedMode
{
    PerMinute = 94,
    PerRevolution
};

LEFT_STREAM_OP_DECL(EFeedMode)
RIGHT_STREAM_OP_DECL(EFeedMode)

enum class ERotationDirection
{
    Right = 3, // M03, CLW (clockwise)
    Left,      // M04, CCLW (counterclockwise)
    Stop,      // M05
};

LEFT_STREAM_OP_DECL(ERotationDirection)
RIGHT_STREAM_OP_DECL(ERotationDirection)

enum class EProgrammingType
{
    Absolute    = 90,
    Incremental = 91
};

LEFT_STREAM_OP_DECL(EProgrammingType)
RIGHT_STREAM_OP_DECL(EProgrammingType)

enum class EDepthProgrammingType
{
    Absolute    = 0,
    Incremental = 1,
};

LEFT_STREAM_OP_DECL(EDepthProgrammingType)
RIGHT_STREAM_OP_DECL(EDepthProgrammingType)

enum class EDrillGmode
{
    G73 = 73,
    G74 = 74,
    G76 = 76,
    G81 = 81,
    G82 = 82,
    G83 = 83,
    G84 = 84,
    G85 = 85,
    G86 = 86,
    G87 = 87,
    G88 = 88,
    G89 = 89
};

LEFT_STREAM_OP_DECL(EDrillGmode)
RIGHT_STREAM_OP_DECL(EDrillGmode)

enum class EDrillGreturnMode
{
    G98 = 98,
    G99 = 99
};

LEFT_STREAM_OP_DECL(EDrillGreturnMode)
RIGHT_STREAM_OP_DECL(EDrillGreturnMode)

enum class EConstantSurfaceSpeedControl
{
    G96 = 96,
    G97 = 97,
};

LEFT_STREAM_OP_DECL(EConstantSurfaceSpeedControl)
RIGHT_STREAM_OP_DECL(EConstantSurfaceSpeedControl)

struct CncDefaultValues
{
    EMotion               default_motion;
    EWorkPlane            default_work_plane;
    EDriverUnits          default_driver_units;
    EProgrammingType      default_programming;
    EFeedMode             default_feed_mode;
    ERotationDirection    default_rotation_direction;
    EDepthProgrammingType drill_cycle_z_value;
    EDrillGreturnMode     drill_cycle_return_value;
    int                   default_rotation;
    bool                  rapid_traverse_cancel_cycle;
    bool                  linear_interpolation_cancel_cycle;
    bool                  circular_interpolation_cw_cancel_cycle;
    bool                  circular_interpolation_ccw_cancel_cycle;
    bool                  cycle_cancel_starts_rapid_traverse;
    bool                  operator_turns_on_rotation;
    bool                  tool_number_executes_exchange;
    bool                  auto_rapid_traverse_after_tool_exchange;
};

struct PathResult
{
    double                    total;
    double                    work_motion; // G1 G2 G3
    double                    fast_motion; // G0
    double                    tool_total;
    std::string               tool_id;
    std::pair<double, double> cycle_distance;
    unsigned int              hole_number;
};

struct TimeResult
{
    double      total;
    double      work_motion; // G1 G2 G3
    double      fast_motion; // G0
    double      tool_total;
    std::string tool_id;
};

enum class UnitConversionType
{
    metric_to_imperial = 0,
    imperial_to_metric
};

LEFT_STREAM_OP_DECL(UnitConversionType)
RIGHT_STREAM_OP_DECL(UnitConversionType)

struct RenumberSettings
{
    std::string  word;
    unsigned int step;
    unsigned int frequency;
    unsigned int max_number;
    bool         existing_only;
    bool         leading_zeroes;
};

enum class AxesRotatingOption
{
    Xrotate90degrees = 0,
    Xrotate180degrees,
    Xrotate270degrees,
    Yrotate90degrees,
    Yrotate180degrees,
    Yrotate270degrees,
    Zrotate90degrees,
    Zrotate180degrees,
    Zrotate270degrees,
};

LEFT_STREAM_OP_DECL(AxesRotatingOption)
RIGHT_STREAM_OP_DECL(AxesRotatingOption)

enum class FileSplittingType
{
    block_numbers,
    tool_change,
    work_motion_time,
    work_motion_path,
    text
};

struct FileSplitting
{
    FileSplittingType type;
    int               step;
    int               time;
    int               path;
    std::string       text;
    bool              rapid_traverse_only;
    std::string       retraction_plane;
};

} // namespace parser
