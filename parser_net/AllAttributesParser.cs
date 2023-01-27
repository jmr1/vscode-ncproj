using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace parser
{
    using MacroValuesDictionary = Dictionary<MacroValuesKey, double>;
    using WordResultRangeList = List<WordResultRange>;

    public enum ParserType
    {
        Fanuc,
        Heidenhain,
    }

    public enum FanucParserType
    {
        FanucLatheSystemA,
        FanucLatheSystemB,
        FanucLatheSystemC,
        FanucMill,
        GenericLathe,
        GenericMill,
        HaasLathe,
        HaasMill,
		MakinoMill
    }

    public enum MachineToolType
    {
        mill,
        lathe,
        millturn
    }

    [StructLayout(LayoutKind.Sequential)]
    public class WordResultRange
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string word;
        public double value_min;
        public double value;
        public double value_max;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class PathResult
    {
        public double total;
        public double work_motion;
        public double fast_motion;
        public double tool_total;
        [MarshalAs(UnmanagedType.LPStr)]
        public string tool_id;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class TimeResult
    {
        public double total;
        public double work_motion;
        public double fast_motion;
        public double tool_total;
        [MarshalAs(UnmanagedType.LPStr)]
        public string tool_id;
    }

    public class WordResultRangeComparer : IEqualityComparer<WordResultRange>
    {
        public bool Equals(WordResultRange pr1, WordResultRange pr2)
        {
            const double tolerance = 1e-8;
            if (Math.Abs(pr1.value - pr2.value) < tolerance &&
                Math.Abs(pr1.value_max - pr2.value_max) < tolerance &&
                Math.Abs(pr1.value_min - pr2.value_min) < tolerance &&
                pr1.word == pr2.word)
                return true;
            return false;
        }

        public int GetHashCode(WordResultRange pr)
        {
            uint hCode = (uint)pr.value ^ (uint)pr.value_max ^ (uint)pr.value_min;
            return hCode.GetHashCode();
        }
    }

    public class PathResultComparer : IEqualityComparer<PathResult>
    {
        public bool Equals(PathResult pr1, PathResult pr2)
        {
            const double tolerance = 1e-8;
            if (Math.Abs(pr1.total - pr2.total) < tolerance &&
               Math.Abs(pr1.work_motion - pr2.work_motion) < tolerance &&
               Math.Abs(pr1.fast_motion - pr2.fast_motion) < tolerance &&
               Math.Abs(pr1.tool_total - pr2.tool_total) < tolerance &&
               pr1.tool_id == pr2.tool_id)
                return true;
            return false;
        }

        public int GetHashCode(PathResult pr)
        {
            uint hCode = (uint)pr.total ^ (uint)pr.work_motion ^ (uint)pr.fast_motion ^ (uint)pr.tool_total;
            return hCode.GetHashCode();
        }
    }

    public class TimeResultComparer : IEqualityComparer<TimeResult>
    {
        public bool Equals(TimeResult pr1, TimeResult pr2)
        {
            const double tolerance = 1e-8;
            if (Math.Abs(pr1.total - pr2.total) < tolerance &&
               Math.Abs(pr1.work_motion - pr2.work_motion) < tolerance &&
               Math.Abs(pr1.fast_motion - pr2.fast_motion) < tolerance &&
               Math.Abs(pr1.tool_total - pr2.tool_total) < tolerance &&
               pr1.tool_id == pr2.tool_id)
                return true;
            return false;
        }

        public int GetHashCode(TimeResult pr)
        {
            uint hCode = (uint)pr.total ^ (uint)pr.work_motion ^ (uint)pr.fast_motion ^ (uint)pr.tool_total;
            return hCode.GetHashCode();
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CodeGroupValueWrapper
    {
        public uint code;
        public uint rest;

        public static explicit operator CodeGroupValueWrapper(CodeGroupValue codeGroupValue)
        {
            var codeGroupValueWraper = new CodeGroupValueWrapper();
            codeGroupValueWraper.code = codeGroupValue.code;
            codeGroupValueWraper.rest = codeGroupValue.rest;
            return codeGroupValueWraper;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct MacroValuesKey
    {
        public int id;
        public int line;
    }

    public enum WordType
    {
        misc = 0,
        angle,
        calc,
        code,
        feed,
        length,
        macro,
        numbering,
        assignable
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct WordGrammarWrapper
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string word;
        public int range_from;
        public int range_to;
        public uint decimal_from;
        public uint decimal_to;
        [MarshalAs(UnmanagedType.I1)]
        public bool unique;
        public WordType type;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct WordValueWrapper
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string word;
        [MarshalAs(UnmanagedType.LPStr)]
        public string value;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct RenumberSettings
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string word;
        public uint step;
        public uint frequency;
        public uint max_number;
        [MarshalAs(UnmanagedType.I1)]
        public bool existing_only;
        [MarshalAs(UnmanagedType.I1)]
        public bool leading_zeroes;
    }


    public enum UnitConversionType
    {
        metric_to_imperial = 0,
        imperial_to_metric
    }

    public enum DepthProgrammingType
    {
        absolute = 0,
        incremental
    }

    public enum AxesRotatingOption
    {
        unset = -1,
        Xrotate90degrees = 0,
        Xrotate180degrees,
        Xrotate270degrees,
        Yrotate90degrees,
        Yrotate180degrees,
        Yrotate270degrees,
        Zrotate90degrees,
        Zrotate180degrees,
        Zrotate270degrees,
    }

    public class AxisParameters
    {
        public double range_min { get; set; }
        public double range_max { get; set; }
    }

    public class MachinePointsData
    {
        public Dictionary<string, double> tool_exchange_point { get; set; }
        public Dictionary<string, double> machine_base_point { get; set; }
    }

    public class Kinematics
    {
        public Dictionary<string, AxisParameters> cartesian_system_axis { get; set; }
        public double max_working_feed { get; set; }
        public double max_fast_feed { get; set; }
        public int maximum_spindle_speed { get; set; }
        public int numer_of_items_in_the_warehouse { get; set; }
        public int tool_exchange_time { get; set; }
        public int pallet_exchange_time { get; set; }
        public int tool_measurement_time { get; set; }
        public bool diameter_programming_2x { get; set; }
        public bool auto_measure_after_tool_selection { get; set; }
        public string pallet_exchange_code { get; set; }
        public string pallet_exchange_code_value { get; set; }
        public string tool_measurement_code { get; set; }
        public string tool_measurement_code_value { get; set; }
    }

    public class CncDefaultValues
    {
        [JsonConverter(typeof(StringEnumConverter))]
        public Motion default_motion { get; set; }
        [JsonConverter(typeof(StringEnumConverter))]
        public WorkPlane default_work_plane { get; set; }
        [JsonConverter(typeof(StringEnumConverter))]
        public DriverUnits default_driver_units { get; set; }
        [JsonConverter(typeof(StringEnumConverter))]
        public ProgrammingType default_programming { get; set; }
        [JsonConverter(typeof(StringEnumConverter))]
        public FeedMode default_feed_mode { get; set; }
        [JsonConverter(typeof(StringEnumConverter))]
        public RotationDirection default_rotation_direction { get; set; }
        [JsonConverter(typeof(StringEnumConverter))]
        public DepthProgrammingType drill_cycle_z_value { get; set; }
        [JsonConverter(typeof(StringEnumConverter))]
        public DrillCycleReturnValue drill_cycle_return_value { get; set; }
        public int default_rotation { get; set; }
        public bool rapid_traverse_cancel_cycle { get; set; }
        public bool linear_interpolation_cancel_cycle { get; set; }
        public bool circular_interpolation_cw_cancel_cycle { get; set; }
        public bool circular_interpolation_ccw_cancel_cycle { get; set; }
        public bool cycle_cancel_starts_rapid_traverse { get; set; }
        public bool operator_turns_on_rotation { get; set; }
        public bool tool_number_executes_exchange { get; set; }
        public bool auto_rapid_traverse_after_tool_exchange { get; set; }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct AxisParametersWrapper
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string axis;
        public double range_min;
        public double range_max;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct KinematicsWrapper
    {
        public double max_working_feed;
        public double max_fast_feed;
        public int maximum_spindle_speed;
        public int numer_of_items_in_the_warehouse;
        public int tool_exchange_time;
        public int pallet_exchange_time;
        public int tool_measurement_time;
        [MarshalAs(UnmanagedType.I1)]
        public bool diameter_programming_2x;
        [MarshalAs(UnmanagedType.I1)]
        public bool auto_measure_after_tool_selection;
        [MarshalAs(UnmanagedType.LPStr)]
        public string pallet_exchange_code;
        [MarshalAs(UnmanagedType.LPStr)]
        public string pallet_exchange_code_value;
        [MarshalAs(UnmanagedType.LPStr)]
        public string tool_measurement_code;
        [MarshalAs(UnmanagedType.LPStr)]
        public string tool_measurement_code_value;
    }

    public enum Motion
    {
        rapid_traverse = 0, // G0
        linear_interpolation // G1
    }

    public enum WorkPlane
    {
        XY = 17,
        XZ,
        YZ
    }

    public enum DriverUnits
    {
        millimeter,
        inch
    }

    public enum ProgrammingType
    {
        absolute = 90,
        incremental
    }

    public enum FeedMode
    {
        feed_per_minute = 94,
        feed_per_revolution
    }

    public enum RotationDirection
    {
        right = 3,  // M03, CLW (clockwise)
        left,       // M04, CCLW (counterclockwise)
        stop        // M05
    }

    public enum DrillCycleReturnValue
    {
        beginning = 98,
        point_r
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CncDefaultValuesWrapper
    {
        public Motion default_motion;
        public WorkPlane default_work_plane;
        public DriverUnits default_driver_units;
        public ProgrammingType default_programming;
        public FeedMode default_feed_mode;
        public RotationDirection default_rotation_direction;
        public DepthProgrammingType drill_cycle_z_value;
        public DrillCycleReturnValue drill_cycle_return_value;
        public int default_rotation;
        [MarshalAs(UnmanagedType.I1)]
        public bool rapid_traverse_cancel_cycle;
        [MarshalAs(UnmanagedType.I1)]
        public bool linear_interpolation_cancel_cycle;
        [MarshalAs(UnmanagedType.I1)]
        public bool circular_interpolation_cw_cancel_cycle;
        [MarshalAs(UnmanagedType.I1)]
        public bool circular_interpolation_ccw_cancel_cycle;
        [MarshalAs(UnmanagedType.I1)]
        public bool cycle_cancel_starts_rapid_traverse;
        [MarshalAs(UnmanagedType.I1)]
        public bool operator_turns_on_rotation;
        [MarshalAs(UnmanagedType.I1)]
        public bool tool_number_executes_exchange;
        [MarshalAs(UnmanagedType.I1)]
        public bool auto_rapid_traverse_after_tool_exchange;
    }

    public enum FileSplittingType
    {
        block_numbers,
        tool_change,
        work_motion_time,
        work_motion_path,
        text
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct FileSplitting
    {
        public FileSplittingType type;
        public int step;
        public int time;
        public int path;
        [MarshalAs(UnmanagedType.LPStr)]
        public string text;
        [MarshalAs(UnmanagedType.I1)]
        public bool rapid_traverse_only;
        [MarshalAs(UnmanagedType.LPStr)]
        public string retraction_plane;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ZeroPoint
    {
        public double x;
        public double y;
        public double z;
    }

    public struct ParserSettings
    {
        public bool parse;
        public bool evaluate_macro;
        public bool verify_code_groups;
        public bool calculate_path;
        public bool ncsettings_code_analysis;
        public bool zero_point_analysis;
    }
    public enum Language
    {
        English,
        Polish,
    }

    public class StringLanguageEnumConverter : StringEnumConverter
    {
        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            var value = (string)reader.Value;

            if (string.IsNullOrEmpty(value))
            {
                var currentCulture = Thread.CurrentThread.CurrentUICulture;
                if (currentCulture.Name == "pl-PL" || currentCulture.TwoLetterISOLanguageName == "pl")
                    return Language.Polish;
                if (currentCulture.TwoLetterISOLanguageName == "en")
                    return Language.English;
                return Language.English;
            }

            return base.ReadJson(reader, objectType, existingValue, serializer);
        }
    }

    public struct OtherSettings
    {
        [JsonConverter(typeof(StringLanguageEnumConverter))]
        public Language language;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct OtherSettingsWrapper
    {
        public Language language;

        public static explicit operator OtherSettingsWrapper(OtherSettings otherSettings)
        {
            var otherSettingsWrapper = new OtherSettingsWrapper();
            otherSettingsWrapper.language = otherSettings.language;
            return otherSettingsWrapper;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ParserSettingsWrapper
    {
        [MarshalAs(UnmanagedType.I1)]
        public bool evaluate_macro;
        [MarshalAs(UnmanagedType.I1)]
        public bool verify_code_groups;
        [MarshalAs(UnmanagedType.I1)]
        public bool calculate_path;
        [MarshalAs(UnmanagedType.I1)]
        public bool ncsettings_code_analysis;
        [MarshalAs(UnmanagedType.I1)]
        public bool zero_point_analysis;

        public static explicit operator ParserSettingsWrapper(ParserSettings parserSettings)
        {
            var parserSettingsWrapper = new ParserSettingsWrapper();
            parserSettingsWrapper.evaluate_macro = parserSettings.evaluate_macro;
            parserSettingsWrapper.verify_code_groups = parserSettings.verify_code_groups;
            parserSettingsWrapper.calculate_path = parserSettings.calculate_path;
            parserSettingsWrapper.ncsettings_code_analysis = parserSettings.ncsettings_code_analysis;
            parserSettingsWrapper.zero_point_analysis = parserSettings.zero_point_analysis;
            return parserSettingsWrapper;
        }
    }

    public class AllAttributesParser : IDisposable
    {
        private readonly int wordValueWrapperSize = Marshal.SizeOf(typeof(WordValueWrapper));
        private readonly int wordResultRangeSize = Marshal.SizeOf(typeof(WordResultRange));

#if (!SIMULATOR)

#if WINDOWS
        public const string dllname = "parser.dll";
#else
        public const string dllname = "libparser.so";
#endif

        [DllImport(dllname)]
        static protected extern IntPtr CreateFanucAllAttributesParser(
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] metric_grammar_keys,
            [MarshalAs(UnmanagedType.LPArray)] WordGrammarWrapper[] metric_grammar_values,
            int metric_grammar_length,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] imperial_grammar_keys,
            [MarshalAs(UnmanagedType.LPArray)] WordGrammarWrapper[] imperial_grammar_values,
            int imperial_grammar_length,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] operations,
            int operations_length,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] gcode_groups_keys,
            [MarshalAs(UnmanagedType.LPArray)] CodeGroupValueWrapper[] gcode_groups_values,
            int gcode_groups_length,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I4)] int[] gcode_groups_values_length,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] mcode_groups_keys,
            [MarshalAs(UnmanagedType.LPArray)] CodeGroupValueWrapper[] mcode_groups_values,
            int mcode_groups_length,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I4)] int[] mcode_groups_values_length,
            ref ParserSettingsWrapper parser_settings,
            ref OtherSettingsWrapper other_settings,
            FanucParserType fanuc_parser_type);
#else
        static protected IntPtr CreateFanucAllAttributesParser(
            string[] metric_grammar_keys,
            WordGrammarWrapper[] metric_grammar_values,
            int metric_grammar_length,
            string[] imperial_grammar_keys,
            WordGrammarWrapper[] imperial_grammar_values,
            int imperial_grammar_length,
            string[] operations,
            int operations_length,
            string[] gcode_groups_keys,
            CodeGroupValueWrapper[] gcode_groups_values,
            int gcode_groups_length,
            int[] gcode_groups_values_length,
            string[] mcode_groups_keys,
            CodeGroupValueWrapper[] mcode_groups_values,
            int mcode_groups_length,
            int[] mcode_groups_values_length,
            ref ParserSettingsWrapper parser_settings,
            ref OtherSettingsWrapper other_settings,
            FanucParserType fanuc_parser_type)
        {
            return IntPtr.Zero;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static protected extern IntPtr CreateHeidenhainAllAttributesParser(
            ref ParserSettingsWrapper parser_settings,
            ref OtherSettingsWrapper other_settings);
#else
        static protected IntPtr CreateHeidenhainAllAttributesParser(
            ref ParserSettingsWrapper parser_settings,
            ref OtherSettingsWrapper other_settings)
        {
            return IntPtr.Zero;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void DisposeAllAttributesParser(IntPtr ptr);
#else
        static private void DisposeAllAttributesParser(IntPtr ptr){}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void SetParserSettings(
            IntPtr ptr,
            ref ParserSettingsWrapper parser_settings);
#else
        static private void SetParserSettings(
            IntPtr ptr,
            ref ParserSettingsWrapper parser_settings)
        {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool Parse(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr message,
            bool single_line_msg);
#else
        static private bool Parse(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr message,
            bool single_line_msg)
        {
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void DisposeCharArray(IntPtr ptr);
#else
        static private void DisposeCharArray(IntPtr ptr){}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool ParseValue(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg);
#else
        static private bool ParseValue(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg)
        {
            value = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void DisposeWordValueWrapper(
            IntPtr ptr, 
            int length);
#else
        static private void DisposeWordValueWrapper(
            IntPtr ptr, 
            int length)
        {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void GetMacroValues(
            IntPtr ptr,
            out IntPtr macro_ids,
            out IntPtr macro_lines,
            out IntPtr macro_values,
            out uint length);
#else
        static private void GetMacroValues(
            IntPtr ptr,
            out IntPtr macro_ids,
            out IntPtr macro_lines,
            out IntPtr macro_values,
            out uint length)
        {
            macro_ids = IntPtr.Zero;
            macro_lines = IntPtr.Zero;
            macro_values = IntPtr.Zero;
            length = 0;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void GetLineMacroValues(
            IntPtr ptr,
            int macro_line,
            out IntPtr macro_ids,
            out IntPtr macro_values,
            out uint length);
#else
        static private void GetLineMacroValues(
            IntPtr ptr,
            int macro_line,
            out IntPtr macro_ids,
            out IntPtr macro_values,
            out uint length)
        {
            macro_ids = IntPtr.Zero;
            macro_values = IntPtr.Zero;
            length = 0;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void DisposeMacroValues(
            IntPtr macro_ids, 
            IntPtr macro_lines, 
            IntPtr macro_values);
#else
        static private void DisposeMacroValues(
            IntPtr macro_ids, 
            IntPtr macro_lines, 
            IntPtr macro_values)
        {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void ResetMacroValues(IntPtr ptr);
#else
        static private void ResetMacroValues(IntPtr ptr){}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void GetPathValues(
            IntPtr ptr,
            out IntPtr path_result,
            out IntPtr time_result,
            out IntPtr word_result_range,
            out int length,
            out int unit_system);
#else
        static private void GetPathValues(
            IntPtr ptr,
            out IntPtr path_result,
            out IntPtr time_result,
            out IntPtr word_result_range,
            out int length,
            out int unit_system)
        {
            path_result = IntPtr.Zero;
            time_result = IntPtr.Zero;
            word_result_range = IntPtr.Zero;
            length = 0;
            unit_system = 0;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void DisposePathValues(
            IntPtr path_result,
            IntPtr time_result,
            IntPtr word_result_range,
            int length);
#else
        static private void DisposePathValues(
            IntPtr path_result,
            IntPtr time_result,
            IntPtr word_result_range,
            int length)
        {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void ResetPathValues(IntPtr ptr);
#else
        static private void ResetPathValues(IntPtr ptr) {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool ConvertLength(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg,
            UnitConversionType conversion_type);
#else
        static private bool ConvertLength(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg,
            UnitConversionType conversion_type)
        {
            value = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool RemoveNumbering(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg);
#else
        static private bool RemoveNumbering(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg)
        {
            value = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool Renumber(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg,
            ref RenumberSettings renumber_settings);
#else
        static private bool Renumber(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg,
            ref RenumberSettings renumber_settings)
        {
            value = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void SetRenumberStartValue(
            IntPtr ptr, 
            uint value);
#else
        static private void SetRenumberStartValue(
            IntPtr ptr, 
            uint value) {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void RenumberReset(IntPtr ptr);
#else
        static private void RenumberReset(IntPtr ptr) {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool RotateAxes(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg,
            AxesRotatingOption axesRotatingOptions);
#else
        static private bool RotateAxes(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg,
            AxesRotatingOption axesRotatingOptions)
        {
            value = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void SetNCSettings(
            IntPtr ptr,
            MachineToolType machine_tool_type,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] machine_base_point_default_keys,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.R8)] double[] machine_base_point_default_vals,
            int machine_base_point_default_length,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] tool_exchange_point_default_keys,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.R8)] double[] tool_exchange_point_default_vals,
            int tool_exchange_point_default_length,
            ref KinematicsWrapper kinematics,
            [MarshalAs(UnmanagedType.LPArray)] AxisParametersWrapper[] kinematics_axis_parameters,
            int kinematics_axis_parameters_length,
            ref CncDefaultValuesWrapper cnc_default_values,
            ref ZeroPoint zero_point
            );
#else
        static private void SetNCSettings(
            IntPtr ptr,
            MachineToolType machine_tool_type,
            string[] machine_base_point_default_keys,
            double[] machine_base_point_default_vals,
            int machine_base_point_default_length,
            string[] tool_exchange_point_default_keys,
            double[] tool_exchange_point_default_vals,
            int tool_exchange_point_default_length,
            ref KinematicsWrapper kinematics,
            AxisParametersWrapper[] kinematics_axis_parameters,
            int kinematics_axis_parameters_length,
            ref CncDefaultValuesWrapper cnc_default_values,
            ref ZeroPoint zero_point
            )
        {
            
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool SplitFile(
            IntPtr ptr,
            ref FileSplitting file_splitting,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] data,
            int data_length,
            out IntPtr value,
            out IntPtr value_value_length,  // value[][][x]
            out IntPtr value_length,        // value[][x][]
            out int length,                 // value[x][][]
            out IntPtr message,
            bool single_line_msg);
#else
        static private bool SplitFile(
            IntPtr ptr,
            ref FileSplitting file_splitting,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] data,
            int data_length,
            out IntPtr value,
            out IntPtr value_value_length,  // value[][][x]
            out IntPtr value_length,        // value[][x][]
            out int length,                 // value[x][][]
            out IntPtr message,
            bool single_line_msg)
        {
            value = IntPtr.Zero;
            value_value_length = IntPtr.Zero;
            value_length = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void DisposeSplitFile(
            IntPtr value,
            IntPtr value_value_length,  // value[][][x]
            IntPtr value_length,        // value[][x][]
            int length);
#else
        static private void DisposeSplitFile(
            IntPtr value,
            IntPtr value_value_length,  // value[][][x]
            IntPtr value_length,        // value[][x][]
            int length)
        {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool SplitFileString(
            IntPtr ptr,
            ref FileSplitting file_splitting,
            [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr)] string[] data,
            int data_length,
            out IntPtr value,
            out int length, // value[x][]
            out IntPtr message,
            bool single_line_msg);
#else
        static private bool SplitFileString(
            IntPtr ptr,
            ref FileSplitting file_splitting,
            string[] data,
            int data_length,
            out IntPtr value,
            out int length, // value[x][]
            out IntPtr message,
            bool single_line_msg)
        {
            value = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern void DisposeSplitFileString(
            IntPtr value,
            int length);
#else
        static private void DisposeSplitFileString(
            IntPtr value,
            int length)
        {}
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool RemoveComment(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg);
#else
        static private bool RemoveComment(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg)
        {
            value = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

#if (!SIMULATOR)
        [DllImport(dllname)]
        static private extern bool RemoveOptionalBlock(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg);
#else
        static private bool RemoveOptionalBlock(
            IntPtr ptr,
            int line,
            string data,
            out IntPtr value,
            out int length,
            out IntPtr message,
            bool single_line_msg)
        {
            value = IntPtr.Zero;
            length = 0;
            message = IntPtr.Zero;
            return true;
        }
#endif

        protected IntPtr pAllAttributesParser;

        public void SetParserSettings(ref ParserSettings parserSettings)
        {
            var parserSettingsWrapper = (ParserSettingsWrapper)parserSettings;
            SetParserSettings(pAllAttributesParser, ref parserSettingsWrapper);
        }

        public static IntPtr NativeUtf8FromString(string managedString)
        {
            int len = Encoding.UTF8.GetByteCount(managedString);
            var buffer = new byte[len + 1];
            Encoding.UTF8.GetBytes(managedString, 0, managedString.Length, buffer, 0);
            IntPtr nativeUtf8 = Marshal.AllocHGlobal(buffer.Length);
            Marshal.Copy(buffer, 0, nativeUtf8, buffer.Length);
            return nativeUtf8;
        }

        public static string StringFromNativeUtf8(IntPtr nativeUtf8)
        {
            int len = 0;
            while (Marshal.ReadByte(nativeUtf8, len) != 0) ++len;
            var buffer = new byte[len];
            Marshal.Copy(nativeUtf8, buffer, 0, buffer.Length);
            return Encoding.UTF8.GetString(buffer);
        }

        public bool Parse(int line, 
                          string data, 
                          out string message, 
                          bool single_line_msg = true)
        {
            IntPtr msg;
            var ret = Parse(pAllAttributesParser, line, data, out msg, single_line_msg);

            if(msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            return ret;
        }

        public bool Parse(int line, 
                          string data, 
                          out WordValueWrapper[] value_array, 
                          out string message, 
                          bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            int length = 0;
            var ret = ParseValue(pAllAttributesParser, line, data, out value, out length, out msg, single_line_msg);

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            value_array = new WordValueWrapper[length];

            if(length == 0)
                return ret;

            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                value_array[x] = (WordValueWrapper)Marshal.PtrToStructure(current, typeof(WordValueWrapper));
                current = (IntPtr)((long)current + wordValueWrapperSize);
            }
            DisposeWordValueWrapper(value, length);

            return ret;
        }

        public MacroValuesDictionary GetMacroValues()
        {
            uint size = 0;
            IntPtr macro_ids;
            IntPtr macro_lines;
            IntPtr macro_values;
            GetMacroValues(pAllAttributesParser, out macro_ids, out macro_lines, out macro_values, out size);

            var macro_values_dict = new MacroValuesDictionary();
            if (size == 0)
                return macro_values_dict;

            var mids = new int[size];
            Marshal.Copy(macro_ids, mids, 0, (int)size);
            var mlines = new int[size];
            Marshal.Copy(macro_lines, mlines, 0, (int)size);
            var mv = new double[size];
            Marshal.Copy(macro_values, mv, 0, (int)size);

            for (int x = 0; x < size; ++x)
                macro_values_dict[new MacroValuesKey() { id = mids[x], line = mlines[x] }] = mv[x];

            DisposeMacroValues(macro_ids, macro_lines, macro_values);

            return macro_values_dict;
        }

        public uint GetMacroValues(int line, MacroValuesDictionary macro_values_dict)
        {
            uint size = 0;
            IntPtr macro_ids;
            IntPtr macro_values;
            GetLineMacroValues(pAllAttributesParser, line, out macro_ids, out macro_values, out size);

            if (size == 0)
                return size;

            var mids = new int[size];
            Marshal.Copy(macro_ids, mids, 0, (int)size);
            var mv = new double[size];
            Marshal.Copy(macro_values, mv, 0, (int)size);

            for (int x = 0; x < size; ++x)
                macro_values_dict[new MacroValuesKey() { id = mids[x], line = line }] = mv[x];

            DisposeMacroValues(macro_ids, IntPtr.Zero, macro_values);

            return size;
        }

        public void ResetMacroValues()
        {
            ResetMacroValues(pAllAttributesParser);
        }

        public void GetPathValues(out PathResult path_result, out TimeResult time_result, out WordResultRangeList word_result_range, out DriverUnits driver_units)
        {
            int units = 0;
            int size = 0;
            IntPtr word_values;
            IntPtr path_result_value;
            IntPtr time_result_value;
            GetPathValues(pAllAttributesParser, out path_result_value, out time_result_value, out word_values, out size, out units);

            path_result = path_result_value != IntPtr.Zero ? (PathResult)Marshal.PtrToStructure(path_result_value, typeof(PathResult)) : new PathResult();
            time_result = time_result_value != IntPtr.Zero ? (TimeResult)Marshal.PtrToStructure(time_result_value, typeof(TimeResult)) : new TimeResult();
            driver_units = (DriverUnits)units;

            word_result_range = new WordResultRangeList();
            if (size == 0)
            {
                DisposePathValues(path_result_value, time_result_value, word_values, size);
                return;
            }

            IntPtr current = word_values;
            for (int x = 0; x < size; ++x)
            {
                var res = (WordResultRange)Marshal.PtrToStructure(current, typeof(WordResultRange));
                word_result_range.Add(res);
                current = (IntPtr)((long)current + wordResultRangeSize);
            }
            DisposePathValues(path_result_value, time_result_value, word_values, size);
        }

        public void ResetPathValues()
        {
            ResetPathValues(pAllAttributesParser);
        }

        public bool ConvertLength(int line,
                                  string data,
                                  out WordValueWrapper[] value_array,
                                  out string message,
                                  UnitConversionType conversion_type,
                                  bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            int length = 0;
            var ret = ConvertLength(pAllAttributesParser, line, data, out value, out length, out msg, single_line_msg, conversion_type);

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            value_array = new WordValueWrapper[length];

            if(length == 0)
                return ret;

            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                value_array[x] = (WordValueWrapper)Marshal.PtrToStructure(current, typeof(WordValueWrapper));
                current = (IntPtr)((long)current + wordValueWrapperSize);
            }
            DisposeWordValueWrapper(value, length);

            return ret;
        }

        public bool RemoveNumbering(int line,
                                    string data,
                                    out WordValueWrapper[] value_array,
                                    out string message,
                                    bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            int length = 0;
            var ret = RemoveNumbering(pAllAttributesParser, line, data, out value, out length, out msg, single_line_msg);

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            value_array = new WordValueWrapper[length];

            if(length == 0)
                return ret;

            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                value_array[x] = (WordValueWrapper)Marshal.PtrToStructure(current, typeof(WordValueWrapper));
                current = (IntPtr)((long)current + wordValueWrapperSize);
            }
            DisposeWordValueWrapper(value, length);

            return ret;
        }

        public bool Renumber(int line,
                             string data,
                             out WordValueWrapper[] value_array,
                             out string message,
                             ref RenumberSettings renumber_settings,
                             bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            int length = 0;
            var ret = Renumber(pAllAttributesParser, line, data, out value, out length, out msg, single_line_msg, ref renumber_settings);

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            value_array = new WordValueWrapper[length];

            if(length == 0)
                return ret;

            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                value_array[x] = (WordValueWrapper)Marshal.PtrToStructure(current, typeof(WordValueWrapper));
                current = (IntPtr)((long)current + wordValueWrapperSize);
            }
            DisposeWordValueWrapper(value, length);

            return ret;
        }

        public void SetRenumberStartValue(uint value)
        {
            SetRenumberStartValue(pAllAttributesParser, value);
        }

        public void RenumberReset()
        {
            RenumberReset(pAllAttributesParser);
        }

        public bool RotateAxes(int line,
                                  string data,
                                  out WordValueWrapper[] value_array,
                                  out string message,
                                  AxesRotatingOption axesRotatingOptions,
                                  bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            int length = 0;
            var ret = RotateAxes(pAllAttributesParser, line, data, out value, out length, out msg, single_line_msg, axesRotatingOptions);

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            value_array = new WordValueWrapper[length];

            if(length == 0)
                return ret;

            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                value_array[x] = (WordValueWrapper)Marshal.PtrToStructure(current, typeof(WordValueWrapper));
                current = (IntPtr)((long)current + wordValueWrapperSize);
            }
            DisposeWordValueWrapper(value, length);

            return ret;
        }

        public void SetNCSettings(MachineToolType machineToolType, MachinePointsData machinePointsData, Kinematics kinematics, CncDefaultValues cncDefaultValues, Dictionary<string, double> zeroPointDict)
        {
            var kinematicsWrapper = new KinematicsWrapper()
            {
                max_working_feed = kinematics.max_working_feed,
                max_fast_feed = kinematics.max_fast_feed,
                maximum_spindle_speed = kinematics.maximum_spindle_speed,
                numer_of_items_in_the_warehouse = kinematics.numer_of_items_in_the_warehouse,
                tool_exchange_time = kinematics.tool_exchange_time,
                pallet_exchange_time = kinematics.pallet_exchange_time,
                tool_measurement_time = kinematics.tool_measurement_time,
                diameter_programming_2x = kinematics.diameter_programming_2x,
                auto_measure_after_tool_selection = kinematics.auto_measure_after_tool_selection,
                pallet_exchange_code = kinematics.pallet_exchange_code,
                pallet_exchange_code_value = kinematics.pallet_exchange_code_value,
                tool_measurement_code = kinematics.tool_measurement_code,
                tool_measurement_code_value = kinematics.tool_measurement_code_value
            };

            int cnt = 0;
            var axisParametersWrapper = new AxisParametersWrapper[kinematics.cartesian_system_axis.Count];
            foreach (var item in kinematics.cartesian_system_axis)
            {
                axisParametersWrapper[cnt].axis = item.Key;
                axisParametersWrapper[cnt].range_min = item.Value.range_min;
                axisParametersWrapper[cnt].range_max = item.Value.range_max;
                ++cnt;
            }

            var cncDefaultValuesWrapper = new CncDefaultValuesWrapper()
            {
                auto_rapid_traverse_after_tool_exchange = cncDefaultValues.auto_rapid_traverse_after_tool_exchange,
                circular_interpolation_ccw_cancel_cycle = cncDefaultValues.circular_interpolation_ccw_cancel_cycle,
                circular_interpolation_cw_cancel_cycle = cncDefaultValues.circular_interpolation_cw_cancel_cycle,
                cycle_cancel_starts_rapid_traverse = cncDefaultValues.cycle_cancel_starts_rapid_traverse,
                default_driver_units = cncDefaultValues.default_driver_units,
                default_feed_mode = cncDefaultValues.default_feed_mode,
                default_motion = cncDefaultValues.default_motion,
                default_programming = cncDefaultValues.default_programming,
                default_rotation = cncDefaultValues.default_rotation,
                default_rotation_direction = cncDefaultValues.default_rotation_direction,
                default_work_plane = cncDefaultValues.default_work_plane,
                drill_cycle_return_value = cncDefaultValues.drill_cycle_return_value,
                drill_cycle_z_value = cncDefaultValues.drill_cycle_z_value,
                rapid_traverse_cancel_cycle = cncDefaultValues.rapid_traverse_cancel_cycle,
                linear_interpolation_cancel_cycle = cncDefaultValues.linear_interpolation_cancel_cycle,
                operator_turns_on_rotation = cncDefaultValues.operator_turns_on_rotation,
                tool_number_executes_exchange = cncDefaultValues.tool_number_executes_exchange
            };

            var zeroPoint = new ZeroPoint()
            {
                x = zeroPointDict["X"],
                y = zeroPointDict["Y"],
                z = zeroPointDict["Z"]
            };

            SetNCSettings(pAllAttributesParser,
                          machineToolType,
                          machinePointsData.machine_base_point.Keys.ToArray(),
                          machinePointsData.machine_base_point.Values.ToArray(),
                          machinePointsData.machine_base_point.Count(),
                          machinePointsData.tool_exchange_point.Keys.ToArray(),
                          machinePointsData.tool_exchange_point.Values.ToArray(),
                          machinePointsData.tool_exchange_point.Count(),
                          ref kinematicsWrapper,
                          axisParametersWrapper,
                          axisParametersWrapper.Length,
                          ref cncDefaultValuesWrapper,
                          ref zeroPoint
                          );
        }

        public bool SplitFile(ref FileSplitting file_splitting,
                              IList<string> data,
                              out WordValueWrapper[][][] value_array,
                              out string message,
                              bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            IntPtr vv_length;
            IntPtr v_length;
            int length = 0;
            var ret = SplitFile(pAllAttributesParser, 
                                ref file_splitting, 
                                data.ToArray(), 
                                data.Count(), 
                                out value, 
                                out vv_length, 
                                out v_length, 
                                out length, 
                                out msg, 
                                single_line_msg);

            var value_length = new int[length];
            Marshal.Copy(v_length, value_length, 0, length);

            int cnt = 0;
            foreach (var item in value_length)
                cnt += item;

            var value_value_length = new int[cnt];
            Marshal.Copy(vv_length, value_value_length, 0, cnt);

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            value_array = new WordValueWrapper[length][][];

            if (length == 0)
                return ret;

            cnt = 0;
            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                value_array[x] = new WordValueWrapper[value_length[x]][];
                for (int y = 0; y < value_length[x]; ++y, ++cnt)
                {
                    value_array[x][y] = new WordValueWrapper[value_value_length[cnt]];
                    for (int z = 0; z < value_value_length[cnt]; ++z)
                    {
                        value_array[x][y][z] = (WordValueWrapper)Marshal.PtrToStructure(current, typeof(WordValueWrapper));
                        current = (IntPtr)((long)current + wordValueWrapperSize);
                    }
                }
            }
            DisposeSplitFile(value, vv_length, v_length, length);

            return ret;
        }

        public bool SplitFile(ref FileSplitting file_splitting,
                              IList<string> data,
                              out IList<string> files,
                              out string message,
                              bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            int length = 0;
            var ret = SplitFileString(pAllAttributesParser,
                                      ref file_splitting,
                                      data.ToArray(),
                                      data.Count(),
                                      out value,
                                      out length,
                                      out msg,
                                      single_line_msg);

            files = new List<string>();
            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                var strPtr = (IntPtr)Marshal.PtrToStructure(current, typeof(IntPtr));
                files.Add(Marshal.PtrToStringAnsi(strPtr));
                current = (IntPtr)((long)current + IntPtr.Size);
            }

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            DisposeSplitFileString(value, length);

            return ret;
        }

        public bool RemoveComment(int line,
                                  string data,
                                  out WordValueWrapper[] value_array,
                                  out string message,
                                  bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            int length = 0;
            var ret = RemoveComment(pAllAttributesParser, line, data, out value, out length, out msg, single_line_msg);

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            value_array = new WordValueWrapper[length];

            if (length == 0)
                return ret;

            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                value_array[x] = (WordValueWrapper)Marshal.PtrToStructure(current, typeof(WordValueWrapper));
                current = (IntPtr)((long)current + wordValueWrapperSize);
            }
            DisposeWordValueWrapper(value, length);

            return ret;
        }

        public bool RemoveOptionalBlock(int line,
                                        string data,
                                        out WordValueWrapper[] value_array,
                                        out string message,
                                        bool single_line_msg = true)
        {
            IntPtr msg;
            IntPtr value;
            int length = 0;
            var ret = RemoveOptionalBlock(pAllAttributesParser, line, data, out value, out length, out msg, single_line_msg);

            if (msg != IntPtr.Zero)
            {
                //message = Marshal.PtrToStringAnsi(msg);
                message = StringFromNativeUtf8(msg);
                DisposeCharArray(msg);
            }
            else
            {
                message = "";
            }

            value_array = new WordValueWrapper[length];

            if (length == 0)
                return ret;

            IntPtr current = value;
            for (int x = 0; x < length; ++x)
            {
                value_array[x] = (WordValueWrapper)Marshal.PtrToStructure(current, typeof(WordValueWrapper));
                current = (IntPtr)((long)current + wordValueWrapperSize);
            }
            DisposeWordValueWrapper(value, length);

            return ret;
        }


        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects).
                }

                // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
                // TODO: set large fields to null.
                if(pAllAttributesParser != IntPtr.Zero)
                    DisposeAllAttributesParser(pAllAttributesParser);
                pAllAttributesParser = IntPtr.Zero;

                disposedValue = true;
            }
        }

        // TODO: override a finalizer only if Dispose(bool disposing) above has code to free unmanaged resources.
        ~AllAttributesParser() {
        //   // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(false);
        }

        // This code added to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
            // TODO: uncomment the following line if the finalizer is overridden above.
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}
