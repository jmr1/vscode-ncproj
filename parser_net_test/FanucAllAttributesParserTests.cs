using NUnit.Framework;
using NUnit.Framework.Legacy;
using parser;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace parser.Tests
{
    using WordGrammarMap = Dictionary<string, WordGrammarWrapper>;
    using CodeGroupsMap = Dictionary<string, List<CodeGroupValue>>;
    using WordResultRangeList = List<WordResultRange>;

    [TestFixture()]
    public class FanucAllAttributesParserTests
    {
        private List<string> operations;
        private readonly FanucGrammar fanuc_grammar = new FanucGrammar();
        private CodeGroupsMap gCodeGroupsMap;
        private CodeGroupsMap mCodeGroupsMap;
        private MachinePointsData machinePointsData;
        private Kinematics kinematics;
        private CncDefaultValues cncDefaultValues;
        private Dictionary<string, double> zeroPoint;

        [SetUp()]
        public void BeforeTest()
        {
            operations = new List<string> {
                "SIN", "COS", "TAN", "ATAN", "SQRT", "ABS", "ROUND", "FIX",
                "FUP", "OR", "XOR", "AND", "BIN", "BCD", "OR", "XOR", "AND",
                "IF", "WHILE", "DO", "END", "EQ", "LE" };

            WordGrammarMap wordGrammarMap;
            wordGrammarMap = new WordGrammarMap();
            wordGrammarMap.Add("N", new WordGrammarWrapper() { word = "N", range_from = 1, range_to = 9999, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.numbering });
            wordGrammarMap.Add(":", new WordGrammarWrapper() { word = ":", range_from = 1, range_to = 9999, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.numbering });
            wordGrammarMap.Add("A", new WordGrammarWrapper() { word = "A", range_from = 1, range_to = 9999, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.angle });
            wordGrammarMap.Add("G", new WordGrammarWrapper() { word = "G", range_from = 0, range_to = 99, decimal_from = 0, decimal_to = 9, unique = false, type = WordType.code });
            wordGrammarMap.Add("M", new WordGrammarWrapper() { word = "M", range_from = 1, range_to = 999, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.code });
            wordGrammarMap.Add("X", new WordGrammarWrapper() { word = "X", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = true, type = WordType.length });
            wordGrammarMap.Add("Y", new WordGrammarWrapper() { word = "Y", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = true, type = WordType.length });
            wordGrammarMap.Add("Z", new WordGrammarWrapper() { word = "Z", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = true, type = WordType.length });
            wordGrammarMap.Add("I", new WordGrammarWrapper() { word = "I", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = true, type = WordType.length });
            wordGrammarMap.Add("J", new WordGrammarWrapper() { word = "J", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = true, type = WordType.length });
            wordGrammarMap.Add("K", new WordGrammarWrapper() { word = "K", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = true, type = WordType.length });
            wordGrammarMap.Add("S", new WordGrammarWrapper() { word = "S", range_from = 1, range_to = 99999, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.misc });
            wordGrammarMap.Add("T", new WordGrammarWrapper() { word = "T", range_from = 1, range_to = 999, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.misc });
            wordGrammarMap.Add(",C", new WordGrammarWrapper() { word = ",C", range_from = -99, range_to = 99, decimal_from = 0, decimal_to = 99, unique = false, type = WordType.misc });
            wordGrammarMap.Add("O", new WordGrammarWrapper() { word = "O", range_from = 1, range_to = 9999, decimal_from = 0, decimal_to = 0, unique = true, type = WordType.misc });
            wordGrammarMap.Add("/", new WordGrammarWrapper() { word = "/", range_from = 1, range_to = 9, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.calc });
            wordGrammarMap.Add("#", new WordGrammarWrapper() { word = "#", range_from = 0, range_to = 9999, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.macro });
            wordGrammarMap.Add("=", new WordGrammarWrapper() { word = "=", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = false, type = WordType.calc });
            wordGrammarMap.Add("+", new WordGrammarWrapper() { word = "+", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = false, type = WordType.calc });
            wordGrammarMap.Add("-", new WordGrammarWrapper() { word = "-", range_from = -9999, range_to = 9999, decimal_from = 0, decimal_to = 9999, unique = false, type = WordType.calc });
            wordGrammarMap.Add("GOTO", new WordGrammarWrapper() { word = "GOTO", range_from = 1, range_to = 9999, decimal_from = 0, decimal_to = 0, unique = false, type = WordType.misc });

            fanuc_grammar.metric = wordGrammarMap;
            fanuc_grammar.imperial = wordGrammarMap;

            gCodeGroupsMap = new Dictionary<string, List<CodeGroupValue>>();
            mCodeGroupsMap = new Dictionary<string, List<CodeGroupValue>>();
            gCodeGroupsMap.Add("0", new List<CodeGroupValue>(){
                new CodeGroupValue() { code = 0, rest = 0 },
                new CodeGroupValue() { code = 4, rest = 0 },
                new CodeGroupValue() { code = 5, rest = 1 },
                new CodeGroupValue() { code = 5, rest = 4 }
            });
            gCodeGroupsMap.Add("1", new List<CodeGroupValue>(){
                new CodeGroupValue() { code = 2, rest = 0 },
                new CodeGroupValue() { code = 2, rest = 2 }
            });
            gCodeGroupsMap.Add("6", new List<CodeGroupValue>(){
                new CodeGroupValue() { code = 20, rest = 0 },
                new CodeGroupValue() { code = 21, rest = 0 }
            });
            mCodeGroupsMap.Add("0", new List<CodeGroupValue>(){
                new CodeGroupValue() { code = 14, rest = 0 },
                new CodeGroupValue() { code = 15, rest = 0 }
            });
            mCodeGroupsMap.Add("1", new List<CodeGroupValue>(){
                new CodeGroupValue() { code = 22, rest = 0 },
                new CodeGroupValue() { code = 23, rest = 0 }
            });

            machinePointsData = new MachinePointsData()
            {
                machine_base_point = new Dictionary<string, double>() {
                    {"X", 0},
                    {"Y", 0},
                    {"Z", 0},
                    {"I", 0},
                    {"J", 0},
                    {"K", 0},
                },
                tool_exchange_point = new Dictionary<string, double>() {

                }
            };

            kinematics = new Kinematics()
            {
                cartesian_system_axis = new Dictionary<string, AxisParameters>()
                {
                    { "X", new AxisParameters() { range_min = -500, range_max = 500 } },
                    { "Y", new AxisParameters() { range_min = -500, range_max = 500 } },
                    { "Z", new AxisParameters() { range_min = 0, range_max = 1000 } },
                },
                max_working_feed = 20000,
                max_fast_feed = 20000,
                maximum_spindle_speed = 10000,
                numer_of_items_in_the_warehouse = 30,
                tool_exchange_time = 6,
                pallet_exchange_time = 0,
                tool_measurement_time = 0,
                diameter_programming_2x = false,
                auto_measure_after_tool_selection = false,
                pallet_exchange_code = "",
                pallet_exchange_code_value = "",
                tool_measurement_code = "M",
                tool_measurement_code_value = "123"
            };

            cncDefaultValues = new CncDefaultValues()
            {
                auto_rapid_traverse_after_tool_exchange = false,
                circular_interpolation_ccw_cancel_cycle = false,
                circular_interpolation_cw_cancel_cycle = false,
                cycle_cancel_starts_rapid_traverse = false,
                default_driver_units = DriverUnits.millimeter,
                default_feed_mode = FeedMode.feed_per_minute,
                default_motion = Motion.rapid_traverse,
                default_programming = ProgrammingType.absolute,
                default_rotation = 300,
                default_rotation_direction = RotationDirection.right,
                default_work_plane = WorkPlane.XY,
                drill_cycle_return_value = DrillCycleReturnValue.beginning,
                drill_cycle_z_value = DepthProgrammingType.absolute,
                rapid_traverse_cancel_cycle = false,
                linear_interpolation_cancel_cycle = false,
                operator_turns_on_rotation = false,
                tool_number_executes_exchange = false
            };

            zeroPoint = new Dictionary<string, double>()
            {
                {"X", 0},
                {"Y", 0},
                {"Z", 200}
            };
        }

        [TearDown()]
        public void AfterTest()
        {
        }

        [Test()]
        public void FanucAllAttributesParserTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = true
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            ClassicAssert.IsNotNull(new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill));
        }

        [Test()]
        public void ParseTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = false,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);
            string msg;
            ClassicAssert.AreEqual(true, parser.Parse(1, "A500", out msg, true));
            ClassicAssert.IsEmpty(msg);

            ClassicAssert.AreEqual(false, parser.Parse(1, "AB", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Brak wartości dla adresu [A]", msg);

            ClassicAssert.AreEqual(false, parser.Parse(1, "X100 Y200 X200", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Kod X musi być unikalny w obrębie bloku", msg);
        }

        [Test()]
        public void HaasLatheParseTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = false,
                calculate_path = false,
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.HaasLathe);
            ClassicAssert.IsNotNull(parser);
            string msg;
            ClassicAssert.AreEqual(true, parser.Parse(1, "M10", out msg, true));
            ClassicAssert.IsEmpty(msg);

            ClassicAssert.AreEqual(false, parser.Parse(1, "M10 M20", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Kolejny adres [M] niedozwolony w bloku", msg);
        }

        [Test()]
        public void ParseValueTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = false,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);
            string msg;
            WordValueWrapper[] values;

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            ClassicAssert.AreEqual(true, parser.Parse(1, "%", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("%", values[0].word);
            ClassicAssert.AreEqual("", values[0].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, "O1234", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("O", values[0].word);
            ClassicAssert.AreEqual("1234", values[0].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, "A500 ,C-2.4 X1.055 G04 (This is comment! )", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(5, values.Length);
            ClassicAssert.AreEqual("A", values[0].word);
            ClassicAssert.AreEqual("500", values[0].value);
            ClassicAssert.AreEqual(",C", values[1].word);
            ClassicAssert.AreEqual("-2.4", values[1].value);
            ClassicAssert.AreEqual("X", values[2].word);
            ClassicAssert.AreEqual("1.055", values[2].value);
            ClassicAssert.AreEqual("G", values[3].word);
            ClassicAssert.AreEqual("04", values[3].value);
            ClassicAssert.AreEqual("(", values[4].word);
            ClassicAssert.AreEqual("This is comment! )", values[4].value);

            ClassicAssert.AreEqual(false, parser.Parse(1, "AB", out values, out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Brak wartości dla adresu [A]", msg);

            ClassicAssert.AreEqual(true, parser.Parse(1, "/5 A500", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, values.Length);
            ClassicAssert.AreEqual("/", values[0].word);
            ClassicAssert.AreEqual("5", values[0].value);
            ClassicAssert.AreEqual("A", values[1].word);
            ClassicAssert.AreEqual("500", values[1].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, "A#5 ,C-#2", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, values.Length);
            ClassicAssert.AreEqual("A", values[0].word);
            ClassicAssert.AreEqual("#5", values[0].value);
            ClassicAssert.AreEqual(",C", values[1].word);
            ClassicAssert.AreEqual("-#2", values[1].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, "#4=#5+2", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(3, values.Length);
            ClassicAssert.AreEqual("#", values[0].word);
            ClassicAssert.AreEqual("4", values[0].value);
            ClassicAssert.AreEqual("=", values[1].word);
            ClassicAssert.AreEqual("#5", values[1].value);
            ClassicAssert.AreEqual("+", values[2].word);
            ClassicAssert.AreEqual("2", values[2].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, ",C-[-#1+#2]", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(3, values.Length);
            ClassicAssert.AreEqual(",C", values[0].word);
            ClassicAssert.AreEqual("-[", values[0].value);
            ClassicAssert.AreEqual("-", values[1].word);
            ClassicAssert.AreEqual("#1", values[1].value);
            ClassicAssert.AreEqual("+", values[2].word);
            ClassicAssert.AreEqual("#2]", values[2].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, "#2=BIN[#1]", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(3, values.Length);
            ClassicAssert.AreEqual("#", values[0].word);
            ClassicAssert.AreEqual("2", values[0].value);
            ClassicAssert.AreEqual("=", values[1].word);
            ClassicAssert.AreEqual("", values[1].value);
            ClassicAssert.AreEqual("BIN", values[2].word);
            ClassicAssert.AreEqual("[#1]", values[2].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, "GOTO2", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("GOTO", values[0].word);
            ClassicAssert.AreEqual("2", values[0].value);
        }

        [Test()]
        public void HaasMillParseValueTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = false,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.HaasMill);
            ClassicAssert.IsNotNull(parser);
            string msg;
            WordValueWrapper[] values;

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            ClassicAssert.AreEqual(true, parser.Parse(1, "%", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("%", values[0].word);
            ClassicAssert.AreEqual("", values[0].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, "O12345", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("O", values[0].word);
            ClassicAssert.AreEqual("12345", values[0].value);

            ClassicAssert.AreEqual(true, parser.Parse(1, "M10", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("M", values[0].word);
            ClassicAssert.AreEqual("10", values[0].value);

            ClassicAssert.AreEqual(false, parser.Parse(1, "M10 M20", out values, out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Kolejny adres [M] niedozwolony w bloku", msg);
        }

        [Test()]
        public void MacroCalculationTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            string msg;
            ClassicAssert.AreEqual(true, parser.Parse(1, "#1=5", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(1, "#2=4", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(1, "#3=SQRT[#1+#2]", out msg, true));
            ClassicAssert.IsEmpty(msg);

            var macro_values = parser.GetMacroValues();
            ClassicAssert.AreEqual(3, macro_values.Count());
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 1, line = 1 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(5.0));
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 2, line = 1 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(4.0));
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 3, line = 1 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(3.0));
        }

        [Test()]
        public void MacroFindNearestTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            string msg;
            ClassicAssert.AreEqual(true, parser.Parse(1, "#1=5", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(3, "#2=4", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(5, "#2=8", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(7, "#2=16", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(9, "#3=50", out msg, true));
            ClassicAssert.IsEmpty(msg);

            var macro_values = parser.GetMacroValues();
            ClassicAssert.AreEqual(5, macro_values.Count());
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 1, line = 1 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(5.0));
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 2, line = 3 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(4.0));
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 2, line = 5 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(8.0));
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 2, line = 7 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(16.0));
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 3, line = 9 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(50.0));

            var ret_key = macro_values.Keys.Where(kvp => kvp.id == 2 && kvp.line <= 6).OrderBy(kvp => kvp.line).Last();
            ClassicAssert.AreEqual(2, ret_key.id);
            ClassicAssert.AreEqual(5, ret_key.line);

            ret_key = macro_values.Keys.Where(kvp => kvp.id == 2 && kvp.line <= 3).OrderBy(kvp => kvp.line).Last();
            ClassicAssert.AreEqual(2, ret_key.id);
            ClassicAssert.AreEqual(3, ret_key.line);

            ret_key = macro_values.Keys.Where(kvp => kvp.id == 3 && kvp.line <= 12).OrderBy(kvp => kvp.line).Last();
            ClassicAssert.AreEqual(3, ret_key.id);
            ClassicAssert.AreEqual(9, ret_key.line);

            var r = macro_values.Keys.Where(kvp => kvp.id == 2 && kvp.line <= 2).OrderBy(kvp => kvp.line);
            ClassicAssert.IsFalse(r.Any());
        }

        [Test()]
        public void MacroLineCalculationTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            string msg;
            var macro_values = new Dictionary<MacroValuesKey, double>();

            ClassicAssert.AreEqual(true, parser.Parse(1, "#1=5", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, parser.GetMacroValues(1, macro_values));
            ClassicAssert.AreEqual(1, macro_values.Count());
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 1, line = 1 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(5.0));
            ClassicAssert.AreEqual(true, parser.Parse(1, "#2=4", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, parser.GetMacroValues(1, macro_values));
            ClassicAssert.AreEqual(2, macro_values.Count());
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 2, line = 1 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(4.0));

            ClassicAssert.AreEqual(true, parser.Parse(2, "T01", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(0, parser.GetMacroValues(2, macro_values));
            ClassicAssert.AreEqual(2, macro_values.Count());

            ClassicAssert.AreEqual(true, parser.Parse(3, "#3=SQRT[#1+#2]", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, parser.GetMacroValues(3, macro_values));
            ClassicAssert.AreEqual(3, macro_values.Count());
            ClassicAssert.IsTrue(macro_values.ContainsKey(new MacroValuesKey() { id = 3, line = 3 }));
            ClassicAssert.IsTrue(macro_values.ContainsValue(3.0));
        }

        [Test()]
        public void CodeGroupsTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            string msg;
            ClassicAssert.AreEqual(true, parser.Parse(1, "G04", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(1, "G4 G02", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(false, parser.Parse(1, "G4 G5.1", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("G5.1 i G4 są w tej samej Grupie Kodów = 0", msg);
            ClassicAssert.AreEqual(false, parser.Parse(1, "G10", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("G10 nie występuje w Grupie Kodów", msg);
            ClassicAssert.AreEqual(true, parser.Parse(1, "#1=5.4", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(1, "G2.2 G#1", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(false, parser.Parse(1, "G5.1 G#1", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("G5.4 i G5.1 są w tej samej Grupie Kodów = 0", msg);
            ClassicAssert.AreEqual(false, parser.Parse(1, "G5.1 G#2", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Brak wartości dla #2", msg);

            ClassicAssert.AreEqual(true, parser.Parse(1, "M14", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(1, "M14 M22", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(false, parser.Parse(1, "M14 M15", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("M15 i M14 są w tej samej Grupie Kodów = 0", msg);
            ClassicAssert.AreEqual(false, parser.Parse(1, "M10", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("M10 nie występuje w Grupie Kodów", msg);
            ClassicAssert.AreEqual(true, parser.Parse(1, "#3=15", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(true, parser.Parse(1, "M22 M#3", out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(false, parser.Parse(1, "M14 M#3", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("M15 i M14 są w tej samej Grupie Kodów = 0", msg);
            ClassicAssert.AreEqual(false, parser.Parse(1, "M15 M#2", out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Brak wartości dla #2", msg);
        }

        public static void verify_path_results(PathResult path_result, WordResultRangeList word_result_range, DriverUnits driver_units)
        {
            var pathResultComparer = new PathResultComparer();
            var timeResultComparer = new TimeResultComparer();
            var wordResultRangeComparer = new WordResultRangeComparer();
            ClassicAssert.IsTrue(pathResultComparer.Equals(new PathResult() { total = 0, work_motion = 0, fast_motion = 0, tool_total = 0, tool_id = "4" }, path_result));
            ClassicAssert.AreEqual(6, word_result_range.Count());
            ClassicAssert.IsTrue(wordResultRangeComparer.Equals(word_result_range.Find(item => item.word == "X"), new WordResultRange() { word = "X", value_min = 0, value = 0, value_max = 0 }));
            ClassicAssert.IsTrue(wordResultRangeComparer.Equals(word_result_range.Find(item => item.word == "Y"), new WordResultRange() { word = "Y", value_min = 0, value = 0, value_max = 0 }));
            ClassicAssert.IsTrue(wordResultRangeComparer.Equals(word_result_range.Find(item => item.word == "Z"), new WordResultRange() { word = "Z", value_min = 200, value = 200, value_max = 200 }));
            ClassicAssert.AreEqual(DriverUnits.inch, driver_units);
        }

        public static void verify_word_result_range_axis(WordResultRangeList wordResultRangeList, WordResultRange wordResultRange)
        {
            var wordResultRangeComparer = new WordResultRangeComparer();
            ClassicAssert.IsTrue(wordResultRangeList.Any(item => item.word == wordResultRange.word));
            ClassicAssert.IsTrue(wordResultRangeComparer.Equals(wordResultRangeList.Find(item => item.word == wordResultRange.word), wordResultRange));
        }

        [Test()]
        public void PathTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            parser_settings.calculate_path = true;
            parser.SetParserSettings(ref parser_settings);

            bool ret;
            string msg;
            int line = 0;
            DriverUnits driver_units;
            PathResult path_result;
            TimeResult time_result;
            WordResultRangeList word_result_range;
            var pathResultComparer = new PathResultComparer();
            var timeResultComparer = new TimeResultComparer();

            const double tolerance = 1e-8;

            ret = parser.Parse(++line, "T4", out msg, true);
            if (!ret)
                Console.WriteLine(msg);
            ClassicAssert.AreEqual(true, ret);
            ClassicAssert.IsEmpty(msg);
            parser.GetPathValues(out path_result, out time_result, out word_result_range, out driver_units);
            ClassicAssert.IsTrue(pathResultComparer.Equals(new PathResult() { total = 0, work_motion = 0, fast_motion = 0, tool_total = 0, tool_id = "4" }, path_result));
            ClassicAssert.AreEqual(6, word_result_range.Count());
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "X", value_min = 0, value = 0, value_max = 0 });
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "Y", value_min = 0, value = 0, value_max = 0 });
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "Z", value_min = 200, value = 200, value_max = 200 });
            ClassicAssert.AreEqual(DriverUnits.millimeter, driver_units);

            ret = parser.Parse(++line, "G20", out msg, true);
            if (!ret)
                Console.WriteLine(msg);
            ClassicAssert.AreEqual(true, ret);
            ClassicAssert.IsEmpty(msg);
            parser.GetPathValues(out path_result, out time_result, out word_result_range, out driver_units);
            verify_path_results(path_result, word_result_range, driver_units);

            ret = parser.Parse(++line, "M14", out msg, true);
            if (!ret)
                Console.WriteLine(msg);
            ClassicAssert.AreEqual(true, ret);
            ClassicAssert.IsEmpty(msg);
            parser.GetPathValues(out path_result, out time_result, out word_result_range, out driver_units);
            verify_path_results(path_result, word_result_range, driver_units);

            ret = parser.Parse(++line, "G2", out msg, true);
            if (!ret)
                Console.WriteLine(msg);
            ClassicAssert.AreEqual(true, ret);
            ClassicAssert.IsEmpty(msg);
            parser.GetPathValues(out path_result, out time_result, out word_result_range, out driver_units);
            verify_path_results(path_result, word_result_range, driver_units);

            ret = parser.Parse(++line, "(G2)", out msg, true);
            if (!ret)
                Console.WriteLine(msg);
            ClassicAssert.AreEqual(true, ret);
            ClassicAssert.IsEmpty(msg); // here is a comment, so no error
            parser.GetPathValues(out path_result, out time_result, out word_result_range, out driver_units);
            verify_path_results(path_result, word_result_range, driver_units);

            ret = parser.Parse(++line, "/G2", out msg, true);
            if (!ret)
                Console.WriteLine(msg);
            ClassicAssert.AreEqual(true, ret);
            ClassicAssert.IsEmpty(msg); // here is a comment, so no error
            parser.GetPathValues(out path_result, out time_result, out word_result_range, out driver_units);
            verify_path_results(path_result, word_result_range, driver_units);

            ret = parser.Parse(++line, "G0 X4 Y5 Z2", out msg, true);
            if (!ret)
                Console.WriteLine(msg);
            ClassicAssert.AreEqual(true, ret);
            ClassicAssert.IsEmpty(msg);
            parser.GetPathValues(out path_result, out time_result, out word_result_range, out driver_units);
            ClassicAssert.AreEqual(198.1035082980612, path_result.total, tolerance);
            ClassicAssert.AreEqual(0, path_result.work_motion, tolerance);
            ClassicAssert.AreEqual(198.1035082980612, path_result.fast_motion, tolerance);
            ClassicAssert.AreEqual(198.1035082980612, path_result.tool_total, tolerance);
            ClassicAssert.AreEqual("4", path_result.tool_id);
            ClassicAssert.AreEqual(6, word_result_range.Count());
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "X", value_min = 0, value = 4, value_max = 4 });
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "Y", value_min = 0, value = 5, value_max = 5 });
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "Z", value_min = 2, value = 2, value_max = 200 });
            ClassicAssert.AreEqual(DriverUnits.inch, driver_units);

            ret = parser.Parse(++line, "X3 Y4", out msg, true);
            if (!ret)
                Console.WriteLine(msg);
            ClassicAssert.AreEqual(true, ret);
            ClassicAssert.IsEmpty(msg);
            parser.GetPathValues(out path_result, out time_result, out word_result_range, out driver_units);
            ClassicAssert.AreEqual(199.51772186043431, path_result.total, tolerance);
            ClassicAssert.AreEqual(0, path_result.work_motion, tolerance);
            ClassicAssert.AreEqual(1.4142135623731, path_result.fast_motion, tolerance);
            ClassicAssert.AreEqual(199.51772186043431, path_result.tool_total, tolerance);
            ClassicAssert.AreEqual("4", path_result.tool_id);
            ClassicAssert.AreEqual(6, word_result_range.Count());
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "X", value_min = 0, value = 3, value_max = 4 });
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "Y", value_min = 0, value = 4, value_max = 5 });
            verify_word_result_range_axis(word_result_range, new WordResultRange() { word = "Z", value_min = 2, value = 2, value_max = 200 });
            ClassicAssert.AreEqual(DriverUnits.inch, driver_units);
        }

        [Test()]
        public void ConvertLengthTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = false,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);
            string msg;
            WordValueWrapper[] values;

            ClassicAssert.AreEqual(true, parser.ConvertLength(1, "A500 X1.055", out values, out msg, UnitConversionType.metric_to_imperial, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, values.Length);
            ClassicAssert.AreEqual("A", values[0].word);
            ClassicAssert.AreEqual("500", values[0].value);
            ClassicAssert.AreEqual("X", values[1].word);
            ClassicAssert.AreEqual("0.0415", values[1].value);

            ClassicAssert.AreEqual(true, parser.ConvertLength(1, "X0.2756", out values, out msg, UnitConversionType.imperial_to_metric, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("X", values[0].word);
            ClassicAssert.AreEqual("7.0002", values[0].value);

            ClassicAssert.AreEqual(true, parser.ConvertLength(1, "/5 X6", out values, out msg, UnitConversionType.imperial_to_metric, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, values.Length);
            ClassicAssert.AreEqual("/", values[0].word);
            ClassicAssert.AreEqual("5", values[0].value);
            ClassicAssert.AreEqual("X", values[1].word);
            ClassicAssert.AreEqual("6", values[1].value);
        }

        [Test()]
        public void RemoveNumberingTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = false,
                verify_code_groups = false,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);
            string msg;
            WordValueWrapper[] values;

            ClassicAssert.AreEqual(true, parser.RemoveNumbering(1, "N5 X4", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("X", values[0].word);
            ClassicAssert.AreEqual("4", values[0].value);

            ClassicAssert.AreEqual(true, parser.RemoveNumbering(2, ":6 Y6", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("Y", values[0].word);
            ClassicAssert.AreEqual("6", values[0].value);
        }

        [Test()]
        public void RenumberTest()
        {
            var renumber_settings = new RenumberSettings() {
                word =":", step = 3, frequency = 2, existing_only = true, leading_zeroes = true, max_number = 6
            };

            var parser_settings = new ParserSettings()
            {
                evaluate_macro = false,
                verify_code_groups = false,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);
            string msg;
            WordValueWrapper[] values;

            parser.RenumberReset();

            ClassicAssert.AreEqual(true, parser.Renumber(1, "N5 X4", out values, out msg, ref renumber_settings, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, values.Length);
            ClassicAssert.AreEqual(":", values[0].word);
            ClassicAssert.AreEqual("0001", values[0].value);
            ClassicAssert.AreEqual("X", values[1].word);
            ClassicAssert.AreEqual("4", values[1].value);

            ClassicAssert.AreEqual(true, parser.Renumber(2, ":6 Y6", out values, out msg, ref renumber_settings, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("Y", values[0].word);
            ClassicAssert.AreEqual("6", values[0].value);

            ClassicAssert.AreEqual(true, parser.Renumber(3, "X3", out values, out msg, ref renumber_settings, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("X", values[0].word);
            ClassicAssert.AreEqual("3", values[0].value);

            ClassicAssert.AreEqual(true, parser.Renumber(4, "N7 X3", out values, out msg, ref renumber_settings, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, values.Length);
            ClassicAssert.AreEqual(":", values[0].word);
            ClassicAssert.AreEqual("0004", values[0].value);
            ClassicAssert.AreEqual("X", values[1].word);
            ClassicAssert.AreEqual("3", values[1].value);

            ClassicAssert.AreEqual(true, parser.Renumber(5, "N7 X3", out values, out msg, ref renumber_settings, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("X", values[0].word);
            ClassicAssert.AreEqual("3", values[0].value);

            ClassicAssert.AreEqual(true, parser.Renumber(6, "N8 X3", out values, out msg, ref renumber_settings, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, values.Length);
            ClassicAssert.AreEqual(":", values[0].word);
            ClassicAssert.AreEqual("0001", values[0].value);
            ClassicAssert.AreEqual("X", values[1].word);
            ClassicAssert.AreEqual("3", values[1].value);

            parser.SetRenumberStartValue(5);

            ClassicAssert.AreEqual(true, parser.Renumber(7, "N9 X3", out values, out msg, ref renumber_settings, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("X", values[0].word);
            ClassicAssert.AreEqual("3", values[0].value);

            ClassicAssert.AreEqual(true, parser.Renumber(8, "N10 X3", out values, out msg, ref renumber_settings, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(2, values.Length);
            ClassicAssert.AreEqual(":", values[0].word);
            ClassicAssert.AreEqual("0005", values[0].value);
            ClassicAssert.AreEqual("X", values[1].word);
            ClassicAssert.AreEqual("3", values[1].value);
        }

        [Test()]
        public void RotateAxesTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = false,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);
            string msg;
            WordValueWrapper[] values;

            ClassicAssert.AreEqual(true, parser.RotateAxes(1, "G2 G17 X5 Y3 Z7 I-2 J-3 K-4", out values, out msg, AxesRotatingOption.Xrotate90degrees, false));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(8, values.Length);
            ClassicAssert.AreEqual("G", values[0].word);
            ClassicAssert.AreEqual("2", values[0].value);
            ClassicAssert.AreEqual("G", values[1].word);
            ClassicAssert.AreEqual("18", values[1].value);
            ClassicAssert.AreEqual("X", values[2].word);
            ClassicAssert.AreEqual("5", values[2].value);
            ClassicAssert.AreEqual("Z", values[3].word);
            ClassicAssert.AreEqual("-3", values[3].value);
            ClassicAssert.AreEqual("Y", values[4].word);
            ClassicAssert.AreEqual("7", values[4].value);
            ClassicAssert.AreEqual("I", values[5].word);
            ClassicAssert.AreEqual("-2", values[5].value);
            ClassicAssert.AreEqual("K", values[6].word);
            ClassicAssert.AreEqual("3", values[6].value);
            ClassicAssert.AreEqual("J", values[7].word);
            ClassicAssert.AreEqual("-4", values[7].value);
        }

        [Test()]
        public void KinematicsTest()
        {
            var parser_settings = new ParserSettings()
            {
                parse = true,
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = true,
                ncsettings_code_analysis = true,
                zero_point_analysis = true,
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);
            string msg;
            WordValueWrapper[] values;

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            ClassicAssert.AreEqual(false, parser.Parse(1, "S30000", out values, out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Wartość S [30000] wyższa niż maksymalna dozwolona wartość 10000", msg);

            ClassicAssert.AreEqual(true, parser.Parse(1, "S3000", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);

            ClassicAssert.AreEqual(false, parser.Parse(1, "T50", out values, out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Wartość T [50] wyższa niż maksymalna dozwolona wartość 30", msg);

            ClassicAssert.AreEqual(true, parser.Parse(1, "T05", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);

            ClassicAssert.AreEqual(true, parser.Parse(1, "Z-100", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);

            ClassicAssert.AreEqual(false, parser.Parse(1, "Z-300", out values, out msg, true));
            ClassicAssert.IsNotEmpty(msg);
            ClassicAssert.AreEqual("Wartość Z [-300.000000] poza zakresem [-200.000000, 800.000000]", msg);
        }

        [Test()]
        public void SplitFileTextTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);
            string msg;
            WordValueWrapper[][][] values;

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            var fileSplitting = new FileSplitting()
            {
                path = 0, rapid_traverse_only = false, retraction_plane = "G0 Z400", step = 0, text = "(txt)", time = 0, type = FileSplittingType.text
            };

            var data = new List<string>(){
                "%",
                "O1234",
                "(comment)",
                "G0 X0 Y0 Z0",
                "G1 X1 (txt)",
                "G0 Y3",
                "G1 Z5",
                "G0 Y6 (txt)",
                "M30",
                "%"
            };

            ClassicAssert.AreEqual(true, parser.SplitFile(ref fileSplitting, data, out values, out msg));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(3, values.Length);
            ClassicAssert.AreEqual(5, values[0].Length);
            ClassicAssert.AreEqual(5, values[1].Length);
            ClassicAssert.AreEqual(4, values[2].Length);


            ClassicAssert.AreEqual(1,      values[0][0].Length);
            ClassicAssert.AreEqual("%",    values[0][0][0].word);
            ClassicAssert.AreEqual("",     values[0][0][0].value);

            ClassicAssert.AreEqual(1,      values[0][1].Length);
            ClassicAssert.AreEqual("O",    values[0][1][0].word);
            ClassicAssert.AreEqual("1234", values[0][1][0].value);

            ClassicAssert.AreEqual(1,          values[0][2].Length);
            ClassicAssert.AreEqual("(",        values[0][2][0].word);
            ClassicAssert.AreEqual("comment)", values[0][2][0].value);

            ClassicAssert.AreEqual(5,      values[0][3].Length);
            ClassicAssert.AreEqual("N",    values[0][3][0].word);
            ClassicAssert.AreEqual("1",    values[0][3][0].value);
            ClassicAssert.AreEqual("G",    values[0][3][1].word);
            ClassicAssert.AreEqual("0",    values[0][3][1].value);
            ClassicAssert.AreEqual("X",    values[0][3][2].word);
            ClassicAssert.AreEqual("0",    values[0][3][2].value);
            ClassicAssert.AreEqual("Y",    values[0][3][3].word);
            ClassicAssert.AreEqual("0",    values[0][3][3].value);
            ClassicAssert.AreEqual("Z",    values[0][3][4].word);
            ClassicAssert.AreEqual("0",    values[0][3][4].value);

            ClassicAssert.AreEqual(3,      values[0][4].Length);
            ClassicAssert.AreEqual("N",    values[0][4][0].word);
            ClassicAssert.AreEqual("2",    values[0][4][0].value);
            ClassicAssert.AreEqual("G",    values[0][4][1].word);
            ClassicAssert.AreEqual("0",    values[0][4][1].value);
            ClassicAssert.AreEqual("Z",    values[0][4][2].word);
            ClassicAssert.AreEqual("400",  values[0][4][2].value);
        }

        [Test()]
        public void SplitFileTextTest2()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = false
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);
            string msg;

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            var fileSplitting = new FileSplitting()
            {
                path = 0,
                rapid_traverse_only = false,
                retraction_plane = "G0 Z400",
                step = 0,
                text = "(txt)",
                time = 0,
                type = FileSplittingType.text
            };

            var data = new List<string>(){
                "%",
                "O1234",
                "",
                "(comment)",
                "G0 X0 Y0 Z0",
                "G1 X1 (txt)",
                "G0 Y3",
                "G1 Z5",
                "G0 Y6 (txt)",
                "  ",
                "M30",
                "%"
            };

            const string file1 = "%\r\n" +
                "O1234\r\n" +
                "\r\n" +
                "(comment)\r\n" +
                "N1 G0 X0 Y0 Z0\r\n" +
                "N2 G0 Z400\r\n";
            const string file2 = "N3 G0 Z400\r\n" +
                "N4 G1 X1 (txt)\r\n" +
                "N5 G0 Y3\r\n" +
                "N6 G1 Z5\r\n" +
                "N7 G0 Z400\r\n";
            const string file3 = "N8 G0 Z400\r\n" +
                "N9 G0 Y6 (txt)\r\n" +
                "\r\n" +
                "N10 M30\r\n" +
                "%\r\n";

            IList<string> files;

            ClassicAssert.AreEqual(true, parser.SplitFile(ref fileSplitting, data, out files, out msg));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(3, files.Count);
            ClassicAssert.AreEqual(file1, files.ElementAt(0));
            ClassicAssert.AreEqual(file2, files.ElementAt(1));
            ClassicAssert.AreEqual(file3, files.ElementAt(2));
        }

        [Test()]
        public void RemoveCommentTest()
        {
            var parser_settings = new ParserSettings();

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);
            string msg;
            WordValueWrapper[] values;

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            ClassicAssert.AreEqual(true, parser.RemoveComment(1, "A500 ,C-2.4 X1.055 G04 (This is comment! )", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(4, values.Length);
            ClassicAssert.AreEqual("A", values[0].word);
            ClassicAssert.AreEqual("500", values[0].value);
            ClassicAssert.AreEqual(",C", values[1].word);
            ClassicAssert.AreEqual("-2.4", values[1].value);
            ClassicAssert.AreEqual("X", values[2].word);
            ClassicAssert.AreEqual("1.055", values[2].value);
            ClassicAssert.AreEqual("G", values[3].word);
            ClassicAssert.AreEqual("04", values[3].value);
        }

        [Test()]
        public void RemoveOptionalBlockTest()
        {
            var parser_settings = new ParserSettings();

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);
            string msg;
            WordValueWrapper[] values;

            parser.SetNCSettings(MachineToolType.mill, machinePointsData, kinematics, cncDefaultValues, zeroPoint);

            ClassicAssert.AreEqual(true, parser.RemoveOptionalBlock(1, "/5 A500", out values, out msg, true));
            ClassicAssert.IsEmpty(msg);
            ClassicAssert.AreEqual(1, values.Length);
            ClassicAssert.AreEqual("A", values[0].word);
            ClassicAssert.AreEqual("500", values[0].value);
        }

        [Test()]
        public void DisposeTest()
        {
            var parser_settings = new ParserSettings()
            {
                evaluate_macro = true,
                verify_code_groups = true,
                calculate_path = true
            };

            var other_settings = new OtherSettings()
            {
                language = Language.Polish
            };

            var parser = new FanucAllAttributesParser(fanuc_grammar, operations, gCodeGroupsMap, mCodeGroupsMap, ref parser_settings, ref other_settings, FanucParserType.FanucMill);
            ClassicAssert.IsNotNull(parser);
            parser.Dispose();
        }
    }
}
