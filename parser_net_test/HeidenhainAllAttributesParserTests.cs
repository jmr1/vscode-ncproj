using NUnit.Framework;
using parser;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace parser.Tests
{
    [TestFixture()]
    public class HeidenhainAllAttributesParserTests
    {
        private MachinePointsData machinePointsData;
        private Kinematics kinematics;
        private CncDefaultValues cncDefaultValues;
        private Dictionary<string, double> zeroPoint;

        [SetUp()]
        public void BeforeTest()
        {
            machinePointsData = new MachinePointsData()
            {
                machine_base_point = new Dictionary<string, double>() {
                    {"X", 0},
                    {"Y", 0},
                    {"Z", 0},
                },
                tool_exchange_point = new Dictionary<string, double>()
                {

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
        public void HeidenhainAllAttributesParserTest()
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

            Assert.IsNotNull(new HeidenhainAllAttributesParser(ref parser_settings, ref other_settings));
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

            var parser = new HeidenhainAllAttributesParser(ref parser_settings, ref other_settings);
            Assert.IsNotNull(parser);
            string msg;
            Assert.AreEqual(true, parser.Parse(1, "25 DEP LCT X+10 Y+12 R8 F100 M2", out msg, true));
            Assert.IsEmpty(msg);

            Assert.AreEqual(false, parser.Parse(1, "BEGINPGM hei530_origins MM", out msg, true));
            Assert.IsNotEmpty(msg);
            //Assert.AreEqual("Brak wartości dla adresu [A]", msg);

            /*Assert.AreEqual(false, parser.Parse(1, "X100 Y200 X200", out msg, true));
            Assert.IsNotEmpty(msg);
            Assert.AreEqual("Kod X musi być unikalny w obrębie bloku", msg);*/
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

            var parser = new HeidenhainAllAttributesParser(ref parser_settings, ref other_settings);
            Assert.IsNotNull(parser);
            parser.Dispose();
        }
    }
}
