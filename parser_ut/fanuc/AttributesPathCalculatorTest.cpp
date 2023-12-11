#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <GeneralParserDefines.h>
#include <fanuc/AllAttributesParserDefines.h>
#include <fanuc/AttributesPathCalculator.h>

using namespace parser;

namespace fanuc_test {

const boost::none_t _{boost::none};

class AttributesPathCalculatorTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void prepare_expected_values(fanuc::word_range_map&             expected_word_range,
                                 std::map<std::string, PathResult>& path_value_map, PathResult& expected_value,
                                 std::map<std::string, double>& ewr);

    void verify(fanuc::AttributesPathCalculator& apc, fanuc::word_range_map& expected_word_range,
                std::map<std::string, PathResult>& path_value_map, PathResult&& expected_value, bool expected_ret,
                std::map<std::string, double>&& ewr, std::vector<fanuc::AttributeVariant>&& data, std::string&& msg);

    MachinePointsData machine_points_data;
    Kinematics        kinematics;
    CncDefaultValues  cnc_default_values;
};

void AttributesPathCalculatorTest::SetUp()
{
    machine_points_data = {{}, {{"X", 0.0}, {"Y", 0.0}, {"Z", 0.0}}};
    kinematics          = {{{"X", {-500., 500.}}, {"Y", {-500., 500.}}, {"Z", {0., 1000.}}},
                  20000,
                  20000,
                  10000,
                  30,
                  6,
                  0,
                  0,
                  false,
                  false,
                  "",
                  "",
                  "M",
                  "123"};
    cnc_default_values  = {EMotion::RapidTraverse, // G0
                          EWorkPlane::XY,         // G17
                          EDriverUnits::Millimeter,
                          EProgrammingType::Absolute, // G90
                          EFeedMode::PerMinute,
                          ERotationDirection::Right,
                          EDepthProgrammingType::Absolute,
                          EDrillGreturnMode::G98,
                          false,
                          false,
                          false,
                          false,
                          false,
                          false,
                          false,
                          false};
}

void AttributesPathCalculatorTest::prepare_expected_values(fanuc::word_range_map&             expected_word_range,
                                                           std::map<std::string, PathResult>& path_value_map,
                                                           PathResult&                        expected_value,
                                                           std::map<std::string, double>&     ewr)
{
    for (auto&& it_ewr : ewr)
    {
        if (auto&& it = expected_word_range.find(it_ewr.first); it != std::end(expected_word_range))
        {
            const auto current_value                             = it_ewr.second;
            auto& [expected_min, expected_current, expected_max] = it->second;
            if (current_value < expected_min)
                expected_min = current_value;
            if (current_value > expected_max)
                expected_max = current_value;
            expected_current = current_value;
        }
        else
        {
            expected_word_range[it_ewr.first] = std::make_tuple(it_ewr.second, it_ewr.second, it_ewr.second);
        }
    }

    if (auto it = path_value_map.find(expected_value.tool_id); it != std::end(path_value_map))
    {
        if (expected_value.total != 0.)
            it->second.total = expected_value.total;
        else
            it->second.total += expected_value.work_motion + expected_value.fast_motion;
        it->second.work_motion = expected_value.work_motion;
        it->second.fast_motion = expected_value.fast_motion;
        it->second.tool_total += expected_value.work_motion + expected_value.fast_motion;
    }
    else
    {
        path_value_map.emplace(std::make_pair(expected_value.tool_id, expected_value));
        path_value_map[expected_value.tool_id].total = expected_value.total;
    }
}

void AttributesPathCalculatorTest::verify(fanuc::AttributesPathCalculator&   apc,
                                          fanuc::word_range_map&             expected_word_range,
                                          std::map<std::string, PathResult>& path_value_map,
                                          PathResult&& expected_value, bool expected_ret,
                                          std::map<std::string, double>&&        ewr,
                                          std::vector<fanuc::AttributeVariant>&& data, std::string&& msg)
{
    prepare_expected_values(expected_word_range, path_value_map, expected_value, ewr);

    fanuc::macro_map macro_values;
    if (expected_ret)
        EXPECT_NO_THROW(apc.evaluate(data, EDriverUnits::Millimeter, macro_values, 1)) << msg;
    else
        EXPECT_THROW(apc.evaluate(data, EDriverUnits::Millimeter, macro_values, 1), fanuc::path_calc_exception) << msg;

    const double tolerance = 1e-8;

    auto        pr = apc.get_path_result();
    const auto& ev = path_value_map[expected_value.tool_id];
    EXPECT_EQ(ev.tool_id, pr.tool_id) << msg;
    EXPECT_NEAR(ev.work_motion, pr.work_motion, tolerance) << msg;
    EXPECT_NEAR(ev.fast_motion, pr.fast_motion, tolerance) << msg;
    EXPECT_NEAR(ev.tool_total, pr.tool_total, tolerance) << msg;
    EXPECT_NEAR(ev.total, pr.total, tolerance) << msg;

    auto wr = apc.get_word_range();
    EXPECT_EQ(expected_word_range.size(), wr.size()) << msg;
    for (const auto& it : expected_word_range)
    {
        EXPECT_EQ(size_t(1), wr.count(it.first)) << msg;
        const auto& [expected_min, expected_current, expected_max] = it.second;
        const auto& [min, current, max]                            = wr[it.first];

        EXPECT_NEAR(expected_min, min, tolerance) << msg;
        EXPECT_NEAR(expected_current, current, tolerance) << msg;
        EXPECT_NEAR(expected_max, max, tolerance) << msg;
    }
}

TEST_F(AttributesPathCalculatorTest, G90_absolute_programming)
{
    std::map<std::string, PathResult> pvm;
    fanuc::word_range_map             wrm;
    fanuc::AttributesPathCalculator   apc(EMachineTool::mill_3axis, EMachineToolType::Mill, machine_points_data,
                                        kinematics, cnc_default_values, ELanguage::Polish);

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("4")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T4 M6");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
           "X0 Y0 Z0");

    verify(apc, wrm, pvm, {0, 0, 6.70820393249937, 0, "4"}, true, {{"X", 4}, {"Y", 5}, {"Z", 2}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("4")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("5")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("2")}},
           "G90 G0 X4 Y5 Z2");

    verify(apc, wrm, pvm, {0, 0, 1.4142135623731, 0, "4"}, true, {{"X", 3}, {"Y", 4}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("4")}},
           "X3 Y4");

    // verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {{"X", 5}},
    //       {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("10")},
    //        fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("5")}},
    //       "G10 X5");

    verify(apc, wrm, pvm, {0, 2., 0, 0, "4"}, true, {{"Y", 6}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("2")}},
           "G1 Y6 F2");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, false, {},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("2")}},
           "G2 X2 Y3 I2");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, false, {},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("2")}},
           "G2 G17 X2 Y3 I2");

    verify(apc, wrm, pvm, {0, 3.27356113923605, 0, 0, "4"}, true, {{"X", 2}, {"Y", 3}, {"I", 2}, {"J", 3}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("3")}},
           "G2 G17 X2 Y3 I2 J3");

    verify(apc, wrm, pvm, {0, 2.02657149512673, 0, 0, "4"}, true, {{"Z", 4}, {"K", 3}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("18")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("4")},
            fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("3")}},
           "G18 Z4 K3");

    verify(apc, wrm, pvm, {0, 1.00232940315606, 0, 0, "4"}, true, {{"X", 3}, {"I", 3}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("3")}},
           "G3 X3 I3");

    verify(apc, wrm, pvm, {16.424879532, 0, 0, 0, "5"}, true, {},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("5")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T5 M6");

    verify(apc, wrm, pvm, {0, 1.00323318660525, 0, 0, "5"}, true, {{"X", 2}, {"I", 2}, {"J", 3}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("3")}},
           "G3 G17 X2 Y3 I2 J3");
}

TEST_F(AttributesPathCalculatorTest, Not_crash_test)
{
    std::map<std::string, PathResult> pvm;
    fanuc::word_range_map             wrm;
    fanuc::AttributesPathCalculator   apc(EMachineTool::mill_3axis, EMachineToolType::Mill, machine_points_data,
                                        kinematics, cnc_default_values, ELanguage::Polish);

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("4")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T4 M6");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
           "X0 Y0 Z0");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {}, // <- here should be different Z value: -33.8 but currently
                                                       // calculation in brackets next to axis is not supported
           {
               fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
               fanuc::DecimalAttributeData{"Z", _, _, std::string("[")},
               fanuc::DecimalAttributeData{"-", _, _, _, _, std::string("7"), '.', std::string("8")},
               fanuc::DecimalAttributeData{"-", _, _, _, _, std::string("26"), _, _, std::string("]")},
           },
           "G1 Z[-7.8-26]");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {},
           {
               fanuc::DecimalAttributeData{"F", _, _, std::string("[")},
               fanuc::DecimalAttributeData{"-", _, _, _, _, std::string("2")},
               fanuc::DecimalAttributeData{"+", _, _, _, _, std::string("5"), _, _, std::string("]")},
           },
           "F[-2+5]");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {},
           {
               fanuc::DecimalAttributeData{"S", _, _, std::string("[")},
               fanuc::DecimalAttributeData{"-", _, _, _, _, std::string("2")},
               fanuc::DecimalAttributeData{"+", _, _, _, _, std::string("5"), _, _, std::string("]")},
           },
           "S[-2+5]");
}

TEST_F(AttributesPathCalculatorTest, Helix_360_full_arc_G90_absolute_programming)
{
    std::map<std::string, PathResult> pvm;
    fanuc::word_range_map             wrm;
    fanuc::AttributesPathCalculator   apc(EMachineTool::mill_3axis, EMachineToolType::Mill, machine_points_data,
                                        kinematics, cnc_default_values, ELanguage::Polish);

    verify(apc, wrm, pvm, {0, 0, 0, 0, "2"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T2 M6");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "2"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")}},
           "G17 G90");

    verify(apc, wrm, pvm, {0, 0, 197.576958216286, 0, "2"}, true, {{"X", 162.453}, {"Y", 112.453}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("162"), '.', std::string("453")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("112"), '.', std::string("453")}},
           "G90 X162.453 Y112.453");

    verify(apc, wrm, pvm, {0, 0, 153, 0, "2"}, true, {{"Z", 153}},
           {fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("153"), '.'}}, "Z153.");

    verify(apc, wrm, pvm, {0, 40.2425453638832, 0, 0, "2"}, true, {{"X", 150}, {"Y", 150}, {"I", 50.38}, {"J", 37.547}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("94")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.', std::string("38")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("37"), '.', std::string("547")},
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("250"), '.'}},
           "G94 G02 X150. Y150. I50.38 J37.547 F250.");

    verify(apc, wrm, pvm, {0, 315.726154208045, 0, 0, "2"}, true,
           {{"X", 150}, {"Y", 150}, {"Z", 148}, {"I", 50}, {"J", 0}, {"K", -.796}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("148"), '.'},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"K", _, '-', _, _, _, '.', std::string("796")}},
           "X150. Y150. Z148. I50. J0.0 K-.796");

    verify(apc, wrm, pvm, {0, 315.726154208045, 0, 0, "2"}, true, {{"Z", 143}},
           {fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("143"), '.'}}, "Z143.");

    verify(apc, wrm, pvm, {0, 207.178448542163, 0, 0, "2"}, true, {{"X", 240.451}, {"Y", 120.611}, {"Z", 100}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("240"), '.', std::string("451")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("120"), '.', std::string("611")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("100"), '.'},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"K", _, '-', _, _, _, '.', std::string("796")}},
           "X240.451 Y120.611 Z100. I50. J0.0 K-.796");
}

TEST_F(AttributesPathCalculatorTest, Helix_4x90_arc_G90_absolute_programming)
{
    std::map<std::string, PathResult> pvm;
    fanuc::word_range_map             wrm;
    fanuc::AttributesPathCalculator   apc(EMachineTool::mill_3axis, EMachineToolType::Mill, machine_points_data,
                                        kinematics, cnc_default_values, ELanguage::Polish);

    verify(apc, wrm, pvm, {0, 0, 0, 0, "2"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T2 M6");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "2"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")}},
           "G17 G90");

    verify(apc, wrm, pvm, {0, 0, 254.237397756506, 0, "2"}, true, {{"X", 162.453}, {"Y", 112.453}, {"Z", 160}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("162"), '.', std::string("453")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("112"), '.', std::string("453")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("160"), '.'}},
           "G90 X162.453 Y112.453 Z160.");

    verify(apc, wrm, pvm, {0, 0, 7, 0, "2"}, true, {{"Z", 153}},
           {fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("153"), '.'}}, "Z153.");

    verify(apc, wrm, pvm, {0, 40.2425453638832, 0, 0, "2"}, true, {{"X", 150}, {"Y", 150}, {"I", 50.38}, {"J", 37.547}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.', std::string("38")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("37"), '.', std::string("547")},
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("250"), '.'}},
           "G02 X150. Y150. I50.38 J37.547 F250.");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 200}, {"Y", 200}, {"Z", 151.75}, {"I", 50}, {"J", 0}, {"K", -.796}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("200"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("200"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("151"), '.', std::string("75")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"K", _, '-', _, _, _, '.', std::string("796")}},
           "X200. Y200. Z151.75 I50. J0.0 K-.796");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 250}, {"Y", 150}, {"Z", 150.5}, {"I", 0}, {"J", -50}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("250"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("150"), '.', std::string("5")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"J", _, '-', _, _, std::string("50"), '.'}},
           "X250. Y150. Z150.5 I0.0 J-50.");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 200}, {"Y", 100}, {"Z", 149.25}, {"I", -50}, {"J", 0}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("200"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("100"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("149"), '.', std::string("25")},
            fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, '-', _, _, std::string("0"), '.', std::string("0")}},
           "X200. Y100. Z149.25 I-50. J0.0");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 150}, {"Y", 150}, {"Z", 148}, {"I", 0}, {"J", 50}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("148"), '.'},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("50"), '.'}},
           "X150. Y150. Z148. I0.0 J50.");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 200}, {"Y", 200}, {"Z", 146.75}, {"I", 50}, {"J", 0}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("200"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("200"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("146"), '.', std::string("75")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")}},
           "X200. Y200. Z146.75 I50. J0.0");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 250}, {"Y", 150}, {"Z", 145.5}, {"I", 0}, {"J", -50}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("250"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("150"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("145"), '.', std::string("5")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"J", _, '-', _, _, std::string("50"), '.'}},
           "X250. Y150. Z145.5 I0.0 J-50.");

    verify(apc, wrm, pvm, {0, 78.6026230801615, 0, 0, "2"}, true,
           {{"X", 240.451}, {"Y", 120.611}, {"Z", 145}, {"I", -50}, {"J", 0}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("240"), '.', std::string("451")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("120"), '.', std::string("611")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("145"), '.'},
            fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"K", _, '-', _, _, _, '.', std::string("796")}},
           "X240.451 Y120.611 Z145. I-50. J0.0 K-.796");

    verify(apc, wrm, pvm, {0, 314.159059396097, 0, 0, "2"}, true, {{"I", -40.451}, {"J", 29.389}},
           {fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("40"), '.', std::string("451")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("29"), '.', std::string("389")}},
           "I-40.451 J29.389");
}

TEST_F(AttributesPathCalculatorTest, Helix_Archimedes_G90_absolute_programming)
{
    std::map<std::string, PathResult> pvm;
    fanuc::word_range_map             wrm;
    fanuc::AttributesPathCalculator   apc(EMachineTool::mill_3axis, EMachineToolType::Mill, machine_points_data,
                                        kinematics, cnc_default_values, ELanguage::Polish);

    verify(apc, wrm, pvm, {0, 0, 0, 0, "22"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("22")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T22 M6");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "22"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
           "X0 Y0 Z0");

    verify(apc, wrm, pvm, {0, 0, 40, 0, "22"}, true, {{"X", 40}, {"Y", 0}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("40")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")}},
           "G0 G17 G90 X40 Y0");

    verify(apc, wrm, pvm, {0, 1007.34305856097, 0, 0, "22"}, true, {{"X", 0}, {"Y", 0}, {"I", -40}, {"J", 0}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("40")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Q", _, _, _, _, std::string("5")},
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("3")}},
           "G2 X0 Y0 I-40 J0 Q5 F3");

    verify(apc, wrm, pvm, {0, 631.27282128998, 0, 0, "22"}, true, {{"X", 0}, {"Y", 0}, {"I", -40}, {"J", 0}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("40")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"L", _, _, _, _, std::string("5")}},
           "G2 X0 Y0 I-40 J0 L5");
}

TEST_F(AttributesPathCalculatorTest, G91_incremental_programming)
{
    std::map<std::string, PathResult> pvm;
    fanuc::word_range_map             wrm;
    fanuc::AttributesPathCalculator   apc(EMachineTool::mill_3axis, EMachineToolType::Mill, machine_points_data,
                                        kinematics, cnc_default_values, ELanguage::Polish);

    //     verify(apc, wrm, pvm, {0, 0, 0, 0, "0"}, false, {{"X", 0}, {"Y", 0}, {"Z", 0}},
    //            {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
    //             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
    //             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
    //            "G1 X4 Y5 Z2");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("4")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T4 M6");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
           "X0 Y0 Z0");

    verify(apc, wrm, pvm, {0, 0, 6.70820393249937, 0, "4"}, true, {{"X", 4}, {"Y", 5}, {"Z", 2}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("4")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("5")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("2")}},
           "G91 G17 G0 X4 Y5 Z2");

    verify(apc, wrm, pvm, {0, 0, 1.4142135623731, 0, "4"}, true, {{"X", 3}, {"Y", 4}},
           {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("1")},
            fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("1")}},
           "X-1 Y-1");

    //     verify(apc, wrm, pvm, {0, 0, 0, 0, "4"}, true, {{"X", 5}},
    //            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("10")},
    //             fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("5")}},
    //            "G10 X5");

    verify(apc, wrm, pvm, {0, 2., 0, 0, "4"}, true, {{"Y", 6}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("5")}},
           "G1 Y2 F5");

    verify(apc, wrm, pvm, {0, 60.4588444785463, 0, 0, "4"}, true,
           {{"X", 28}, {"Y", -37.301}, {"I", 0.}, {"J", -28.868}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("25"), '.'},
            fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("43"), '.', std::string("301")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("0"), '.'},
            fanuc::DecimalAttributeData{"J", _, '-', _, _, std::string("28"), '.', std::string("868")}},
           "G2 X25. Y-43.301 I0. J-28.868");

    //     verify(apc, wrm, pvm, {0, 120.921356451218, 0, 0, "4"}, true, {{"X", 53}, {"Y", 6.}, {"I", 25.},
    //     {"J", 14.434}},
    //            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
    //             fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("25")},
    //             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("43"), '.', std::string("301")},
    //             fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("25"), '.'},
    //             fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("14"), '.', std::string("434")}},
    //            "G3 X25 Y43.301 I25. J14.434");

    //     verify(apc, wrm, pvm, {0, 120.921356451218, 0, 0, "4"}, true, {{"X", 78}, {"Y", 49.301}},
    //            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
    //             fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("25")},
    //             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("43"), '.', std::string("301")}},
    //            "G3 X25 Y43.301");

    //     verify(apc, wrm, pvm, {0, 181.380700684312, 0, 0, "4"}, true, {},
    //            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
    //             fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("25"), '.'},
    //             fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("14"), '.', std::string("434")}},
    //            "G3 I25. J14.434");
}

TEST_F(AttributesPathCalculatorTest, Helix_360_full_arc_G91_incremental_programming)
{
    std::map<std::string, PathResult> pvm;
    fanuc::word_range_map             wrm;
    fanuc::AttributesPathCalculator   apc(EMachineTool::mill_3axis, EMachineToolType::Mill, machine_points_data,
                                        kinematics, cnc_default_values, ELanguage::Polish);

    verify(apc, wrm, pvm, {0, 0, 0, 0, "2"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T2 M6");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "2"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")}},
           "G17 G91");

    verify(apc, wrm, pvm, {0, 0, 197.576958216286, 0, "2"}, true, {{"X", 162.453}, {"Y", 112.453}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("162"), '.', std::string("453")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("112"), '.', std::string("453")}},
           "G91 X162.453 Y112.453");

    verify(apc, wrm, pvm, {0, 0, 153, 0, "2"}, true, {{"Z", 153}},
           {fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("153"), '.'}}, "Z153.");

    verify(apc, wrm, pvm, {0, 40.2425453638832, 0, 0, "2"}, true, {{"X", 150}, {"Y", 150}, {"I", 50.38}, {"J", 37.547}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("94")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")},
            fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("12"), '.', std::string("453")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("37"), '.', std::string("547")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.', std::string("38")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("37"), '.', std::string("547")},
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("250"), '.'}},
           "G94 G02 X-12.453 Y37.547 I50.38 J37.547 F250.");

    verify(apc, wrm, pvm, {0, 315.726154208045, 0, 0, "2"}, true,
           {{"X", 150}, {"Y", 150}, {"Z", 148}, {"I", 50}, {"J", 0}, {"K", -.796}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0"), '.'},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("5"), '.'},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"K", _, '-', _, _, _, '.', std::string("796")}},
           "X0. Y0. Z-5. I50. J0.0 K-.796");

    verify(apc, wrm, pvm, {0, 315.726154208045, 0, 0, "2"}, true, {{"Z", 143}},
           {fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("5"), '.'}}, "Z-5.");

    verify(apc, wrm, pvm, {0, 207.178448542163, 0, 0, "2"}, true, {{"X", 240.451}, {"Y", 120.611}, {"Z", 100}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("90"), '.', std::string("451")},
            fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("29"), '.', std::string("389")},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("43"), '.'},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"K", _, '-', _, _, _, '.', std::string("796")}},
           "X90.451 Y-29.389 Z-43. I50. J0.0 K-.796");
}

TEST_F(AttributesPathCalculatorTest, Helix_4x90_arc_G91_incremental_programming)
{
    std::map<std::string, PathResult> pvm;
    fanuc::word_range_map             wrm;
    fanuc::AttributesPathCalculator   apc(EMachineTool::mill_3axis, EMachineToolType::Mill, machine_points_data,
                                        kinematics, cnc_default_values, ELanguage::Polish);

    verify(apc, wrm, pvm, {0, 0, 0, 0, "2"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")}},
           "T2 M6");

    verify(apc, wrm, pvm, {0, 0, 0, 0, "2"}, true, {{"X", 0}, {"Y", 0}, {"Z", 0}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")}},
           "G17 G91");

    verify(apc, wrm, pvm, {0, 0, 254.237397756506, 0, "2"}, true, {{"X", 162.453}, {"Y", 112.453}, {"Z", 160}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("162"), '.', std::string("453")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("112"), '.', std::string("453")},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("160"), '.'}},
           "G91 X162.453 Y112.453 Z160.");

    verify(apc, wrm, pvm, {0, 0, 7, 0, "2"}, true, {{"Z", 153}},
           {fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("7"), '.'}}, "Z-7.");

    verify(apc, wrm, pvm, {0, 40.2425453638832, 0, 0, "2"}, true, {{"X", 150}, {"Y", 150}, {"I", 50.38}, {"J", 37.547}},
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")},
            fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("12"), '.', std::string("453")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("37"), '.', std::string("547")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.', std::string("38")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("37"), '.', std::string("547")},
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("250"), '.'}},
           "G02 X-12.453 Y37.547 I50.38 J37.547 F250.");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 200}, {"Y", 200}, {"Z", 151.75}, {"I", 50}, {"J", 0}, {"K", -.796}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("25")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"K", _, '-', _, _, _, '.', std::string("796")}},
           "X50. Y50. Z-1.25 I50. J0.0 K-.796");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 250}, {"Y", 150}, {"Z", 150.5}, {"I", 0}, {"J", -50}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("25")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"J", _, '-', _, _, std::string("50"), '.'}},
           "X50. Y-50. Z-1.25 I0.0 J-50.");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 200}, {"Y", 100}, {"Z", 149.25}, {"I", -50}, {"J", 0}},
           {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("25")},
            fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")}},
           "X-50. Y-50. Z-1.25 I-50. J0.0");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 150}, {"Y", 150}, {"Z", 148}, {"I", 0}, {"J", 50}},
           {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("25")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("50"), '.'}},
           "X-50. Y50. Z-1.25 I0.0 J50.");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 200}, {"Y", 200}, {"Z", 146.75}, {"I", 50}, {"J", 0}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("25")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")}},
           "X50. Y50. Z-1.25 I50. J0.0");

    verify(apc, wrm, pvm, {0, 78.9315385520114, 0, 0, "2"}, true,
           {{"X", 250}, {"Y", 150}, {"Z", 145.5}, {"I", 0}, {"J", -50}},
           {fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("25")},
            fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"J", _, '-', _, _, std::string("50"), '.'}},
           "X50. Y-50. Z-1.25 I0.0 J-50.");

    verify(apc, wrm, pvm, {0, 78.6026230801615, 0, 0, "2"}, true,
           {{"X", 240.451}, {"Y", 120.611}, {"Z", 145}, {"I", -50}, {"J", 0}},
           {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("9"), '.', std::string("549")},
            fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("29"), '.', std::string("389")},
            fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("0"), '.', std::string("5")},
            fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("50"), '.'},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("0"), '.', std::string("0")},
            fanuc::DecimalAttributeData{"K", _, '-', _, _, _, '.', std::string("796")}},
           "X-9.549 Y-29.389 Z-0.5 I-50. J0.0 K-.796");

    verify(apc, wrm, pvm, {0, 314.159059396097, 0, 0, "2"}, true, {{"I", -40.451}, {"J", 29.389}},
           {fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("40"), '.', std::string("451")},
            fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("29"), '.', std::string("389")}},
           "I-40.451 J29.389");
}

} // namespace fanuc_test
