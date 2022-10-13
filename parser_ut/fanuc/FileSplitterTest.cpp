#include "stdafx.h"

#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "AttributesVisitor.h"
#include <GeneralParserDefines.h>
#include <fanuc/FileSplitter.h>

namespace fanuc_test {

const boost::none_t _{boost::none};

class FileSplitterTest : public ::testing::Test
{
protected:
    void verify(std::vector<std::vector<std::string>>&& data, std::vector<std::vector<std::string>>&& expected_data);
    void verify(std::vector<std::vector<std::vector<fanuc::AttributeVariant>>>&& data,
                std::vector<std::vector<std::vector<fanuc::AttributeVariant>>>&& expected_data);
};

void FileSplitterTest::verify(std::vector<std::vector<std::string>>&& data,
                              std::vector<std::vector<std::string>>&& expected_data)
{
    EXPECT_THAT(data, ::testing::ElementsAreArray(expected_data));
}

void FileSplitterTest::verify(std::vector<std::vector<std::vector<fanuc::AttributeVariant>>>&& data,
                              std::vector<std::vector<std::vector<fanuc::AttributeVariant>>>&& expected_data)
{
    ASSERT_EQ(expected_data.size(), data.size());
    for (size_t x = 0; x < expected_data.size(); ++x)
    {
        ASSERT_EQ(expected_data[x].size(), data[x].size()) << "x:" << x;
        for (size_t y = 0; y < expected_data[x].size(); ++y)
        {
            ASSERT_EQ(expected_data[x][y].size(), data[x][y].size()) << "x:" << x << " y:" << y;
            for (size_t z = 0; z < expected_data[x][y].size(); ++z)
                EXPECT_TRUE(boost::apply_visitor(AttributesVisitor(expected_data[x][y][z]), data[x][y][z]))
                    << "x:" << x << " y:" << y << " z:" << z;
        }
    }
}

TEST_F(FileSplitterTest, Text)
{
    std::vector<std::string> data = {"%",     "O1234",       "",    "(comment)", "G0 X0 Y0 Z0", "G1 X1 (txt)", "G0 Y3",
                                     "G1 Z5", "G0 Y6 (txt)", "M30", "%"};

    std::vector<std::vector<std::string>> expected_data = {{"%", "O1234", "", "(comment)", "G0 X0 Y0 Z0", "G0 Z400"},
                                                           {"G0 Z400", "G1 X1 (txt)", "G0 Y3", "G1 Z5", "G0 Z400"},
                                                           {"G0 Z400", "G0 Y6 (txt)", "M30", "%"}};

    FileSplitting       file_splitting{FileSplittingType::text, 0, 0, 0, "txt", false, "G0 Z400"};
    fanuc::FileSplitter fs{file_splitting, {ELanguage::Polish}};
    verify(fs.evaluate(std::move(data)), std::move(expected_data));
}

TEST_F(FileSplitterTest, BlockNumber)
{
    std::vector<std::vector<fanuc::AttributeVariant>> data = {
        {fanuc::AttributeData<char>{"%", 0}},
        {fanuc::AttributeData<std::string>{"O", "1234"}},
        {fanuc::AttributeData<std::string>{"(", "comment)"}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("1")},
         {fanuc::AttributeData<std::string>{"(", "txt)"}}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("5")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
         {fanuc::AttributeData<std::string>{"(", "txt)"}}},
        {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
        {fanuc::AttributeData<char>{"%", 0}}};

    std::vector<fanuc::AttributeVariant> retraction_plane{
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
        fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("400")}};

    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> expected_data = {
        {{fanuc::AttributeData<char>{"%", 0}},
         {fanuc::AttributeData<std::string>{"O", "1234"}},
         {fanuc::AttributeData<std::string>{"(", "comment)"}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
         retraction_plane},
        {retraction_plane,
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("1")},
          {fanuc::AttributeData<std::string>{"(", "txt)"}}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("5")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
          {fanuc::AttributeData<std::string>{"(", "txt)"}}},
         retraction_plane},
        {retraction_plane,
         {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
         {fanuc::AttributeData<char>{"%", 0}}}};

    CncDefaultValues    cnc_default_values{EMotion::RapidTraverse, // G0
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
    FileSplitting       file_splitting{FileSplittingType::block_numbers, 4, 0, 0, "", false, "G0 Z400"};
    fanuc::FileSplitter fs{file_splitting, {ELanguage::Polish}};
    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> ret;
    fs.evaluate(cnc_default_values, std::move(data), retraction_plane, ret);
    verify(std::move(ret), std::move(expected_data));
}

TEST_F(FileSplitterTest, BlockNumberG0)
{
    std::vector<std::vector<fanuc::AttributeVariant>> data = {
        {fanuc::AttributeData<char>{"%", 0}},
        {fanuc::AttributeData<std::string>{"O", "1234"}},
        {fanuc::AttributeData<std::string>{"(", "comment)"}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("1")},
         {fanuc::AttributeData<std::string>{"(", "txt)"}}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("5")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
         {fanuc::AttributeData<std::string>{"(", "txt)"}}},
        {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
        {fanuc::AttributeData<char>{"%", 0}}};

    std::vector<fanuc::AttributeVariant> retraction_plane{
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
        fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("400")}};

    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> expected_data = {
        {{fanuc::AttributeData<char>{"%", 0}},
         {fanuc::AttributeData<std::string>{"O", "1234"}},
         {fanuc::AttributeData<std::string>{"(", "comment)"}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("1")},
          {fanuc::AttributeData<std::string>{"(", "txt)"}}},
         retraction_plane},
        {retraction_plane,
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("5")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
          {fanuc::AttributeData<std::string>{"(", "txt)"}}},
         {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
         retraction_plane},
        {retraction_plane, {fanuc::AttributeData<char>{"%", 0}}}};

    CncDefaultValues    cnc_default_values{EMotion::RapidTraverse, // G0
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
    FileSplitting       file_splitting{FileSplittingType::block_numbers, 4, 0, 0, "", true, "G0 Z400"};
    fanuc::FileSplitter fs{file_splitting, {ELanguage::Polish}};
    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> ret;
    fs.evaluate(cnc_default_values, std::move(data), retraction_plane, ret);
    verify(std::move(ret), std::move(expected_data));
}

TEST_F(FileSplitterTest, BlockNumberDrillCycle)
{
    std::vector<std::vector<fanuc::AttributeVariant>> data = {
        {fanuc::AttributeData<char>{"%", 0}},
        {fanuc::AttributeData<std::string>{"O", "1234"}},
        {fanuc::AttributeData<std::string>{"(", "comment)"}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("98")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("105")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("135")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("70")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("80")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("250")}},
        {fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("30")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("1")},
         {fanuc::AttributeData<std::string>{"(", "txt)"}}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("5")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
         {fanuc::AttributeData<std::string>{"(", "txt)"}}},
        {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
        {fanuc::AttributeData<char>{"%", 0}}};

    std::vector<fanuc::AttributeVariant> retraction_plane{
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
        fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("400")}};

    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> expected_data = {
        {{fanuc::AttributeData<char>{"%", 0}},
         {fanuc::AttributeData<std::string>{"O", "1234"}},
         {fanuc::AttributeData<std::string>{"(", "comment)"}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
          fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("98")},
          fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("105")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("135")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("70")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("80")},
          fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("250")}},
         {fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("30")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
         retraction_plane},
        {retraction_plane,
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("1")},
          {fanuc::AttributeData<std::string>{"(", "txt)"}}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("5")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
          {fanuc::AttributeData<std::string>{"(", "txt)"}}},
         {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
         retraction_plane},
        {retraction_plane, {fanuc::AttributeData<char>{"%", 0}}}};

    CncDefaultValues    cnc_default_values{EMotion::RapidTraverse, // G0
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
    FileSplitting       file_splitting{FileSplittingType::block_numbers, 5, 0, 0, "", false, "G0 Z400"};
    fanuc::FileSplitter fs{file_splitting, {ELanguage::Polish}};
    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> ret;
    fs.evaluate(cnc_default_values, std::move(data), retraction_plane, ret);
    verify(std::move(ret), std::move(expected_data));
}

TEST_F(FileSplitterTest, ToolChange)
{
    std::vector<std::vector<fanuc::AttributeVariant>> data = {
        {fanuc::AttributeData<char>{"%", 0}},
        {fanuc::AttributeData<std::string>{"O", "1234"}},
        {fanuc::AttributeData<std::string>{"(", "comment)"}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
        {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("01")},
         fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("06")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("1")},
         {fanuc::AttributeData<std::string>{"(", "txt)"}}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
        {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("02")},
         fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("06")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("5")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
         {fanuc::AttributeData<std::string>{"(", "txt)"}}},
        {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
        {fanuc::AttributeData<char>{"%", 0}}};

    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> expected_data = {
        {{fanuc::AttributeData<char>{"%", 0}},
         {fanuc::AttributeData<std::string>{"O", "1234"}},
         {fanuc::AttributeData<std::string>{"(", "comment)"}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
         {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("01")},
          fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("06")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("1")},
          {fanuc::AttributeData<std::string>{"(", "txt)"}}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}}},
        {{fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("02")},
          fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("06")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("5")}},
         {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("6")},
          {fanuc::AttributeData<std::string>{"(", "txt)"}}},
         {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
         {fanuc::AttributeData<char>{"%", 0}}}};

    CncDefaultValues    cnc_default_values{EMotion::RapidTraverse, // G0
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
    FileSplitting       file_splitting{FileSplittingType::tool_change, 0, 0, 0, "", false, ""};
    fanuc::FileSplitter fs{file_splitting, {ELanguage::Polish}};
    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> ret;
    fs.evaluate(cnc_default_values, std::move(data), {}, ret);
    verify(std::move(ret), std::move(expected_data));
}

TEST_F(FileSplitterTest, Path)
{
    std::vector<std::vector<fanuc::AttributeVariant>> data = {
        {fanuc::AttributeData<char>{"%", 0}},
        {fanuc::AttributeData<std::string>{"O", "0081"}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("98")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("28")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
        {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("01")},
         fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("06")}},
        {// G0 G54 G90 G20 S1900 M3
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("54")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("20")},
         fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("1900")},
         fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("3")}},
        {// G0 X7 Y1
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")}},
        {// G0 G43 Z1.1 H1
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("43")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"H", _, _, _, _, std::string("1")}},
        {// G99 G81 X7 Y1 Z0 R1.1 F4
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")}},
        {// G99 G81 Y2 Z0 R1.1 F4 K0
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("2")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")},
         fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("0")}},
        {fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        {// G91 G81 Y1 Z-1.1 R0.1 F4 K3
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("0"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")},
         fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("3")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")}},
        {// G99 G81 X7 Y7 Z0 R1.1 F4
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("7")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")}},
        {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
        {fanuc::AttributeData<char>{"%", 0}}};

    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> expected_data = {
        {
            {fanuc::AttributeData<char>{"%", 0}},
            {fanuc::AttributeData<std::string>{"O", "0081"}},
            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("98")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("28")},
             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
            {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("01")},
             fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("06")}},
            {// G0 G54 G90 G20 S1900 M3
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("54")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("20")},
             fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("1900")},
             fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("3")}},
            {// G0 X7 Y1
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")}},
            {// G0 G43 Z1.1 H1
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("43")},
             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("1"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"H", _, _, _, _, std::string("1")}},
            {// G99 G81 X7 Y1 Z0 R1.1 F4
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
             fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")},
             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")}},
            {// G99 G81 Y2 Z0 R1.1 F4 K0
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("2")},
             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")},
             fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("0")}},
            {fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        },
        {
            {// G91 G81 Y1 Z-1.1 R0.1 F4 K3
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")},
             fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("0"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")},
             fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("3")}},
            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        },
        {{fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")}},
         {// G99 G81 X7 Y7 Z0 R1.1 F4
          fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
          fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("7")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
          fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")}},
         {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
         {fanuc::AttributeData<char>{"%", 0}}}};

    std::vector<PathResult> path_result = {{},       {}, {},       {}, {}, {},       {}, {0, 1.1}, {},
                                           {0, 1.1}, {}, {0, 3.3}, {}, {}, {0, 1.1}, {}, {}};

    CncDefaultValues    cnc_default_values{EMotion::RapidTraverse, // G0
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
    FileSplitting       file_splitting{FileSplittingType::work_motion_path, 0, 0, 2, "", false, ""};
    fanuc::FileSplitter fs{file_splitting, {ELanguage::Polish}};
    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> ret;
    fs.evaluate(cnc_default_values, std::move(data), {}, ret, path_result);
    verify(std::move(ret), std::move(expected_data));
}

TEST_F(FileSplitterTest, Time)
{
    std::vector<std::vector<fanuc::AttributeVariant>> data = {
        {fanuc::AttributeData<char>{"%", 0}},
        {fanuc::AttributeData<std::string>{"O", "0081"}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("98")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("28")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
        {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("01")},
         fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("06")}},
        {// G0 G54 G90 G20 S1900 M3
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("54")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("20")},
         fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("1900")},
         fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("3")}},
        {// G0 X7 Y1
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")}},
        {// G0 G43 Z1.1 H1
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("43")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"H", _, _, _, _, std::string("1")}},
        {// G99 G81 X7 Y1 Z0 R1.1 F4
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")}},
        {// G99 G81 Y2 Z0 R1.1 F4 K0
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("2")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")},
         fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("0")}},
        {fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        {// G91 G81 Y1 Z-1.1 R0.1 F4 K3
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")},
         fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("0"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")},
         fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("3")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")}},
        {// G99 G81 X7 Y7 Z0 R1.1 F4
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
         fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
         fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
         fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("7")},
         fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
         fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
         fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")}},
        {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
        {fanuc::AttributeData<char>{"%", 0}}};

    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> expected_data = {
        {
            {fanuc::AttributeData<char>{"%", 0}},
            {fanuc::AttributeData<std::string>{"O", "0081"}},
            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("98")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("28")},
             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")}},
            {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("01")},
             fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("06")}},
            {// G0 G54 G90 G20 S1900 M3
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("54")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("20")},
             fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("1900")},
             fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("3")}},
            {// G0 X7 Y1
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")}},
            {// G0 G43 Z1.1 H1
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("43")},
             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("1"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"H", _, _, _, _, std::string("1")}},
            {// G99 G81 X7 Y1 Z0 R1.1 F4
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
             fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")},
             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")}},
            {// G99 G81 Y2 Z0 R1.1 F4 K0
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("2")},
             fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
             fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")},
             fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("0")}},
            {fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("3")}},
            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        },
        {
            {// G91 G81 Y1 Z-1.1 R0.1 F4 K3
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("91")},
             fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
             fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("1")},
             fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("1"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("0"), '.', std::string("1")},
             fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")},
             fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("3")}},
            {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("80")}},
        },
        {{fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("90")}},
         {// G99 G81 X7 Y7 Z0 R1.1 F4
          fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("99")},
          fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
          fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("7")},
          fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("7")},
          fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("0")},
          fanuc::DecimalAttributeData{"R", _, _, _, _, std::string("1"), '.', std::string("1")},
          fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("4")}},
         {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}},
         {fanuc::AttributeData<char>{"%", 0}}}};

    std::vector<TimeResult> time_result = {{},        {}, {},        {}, {}, {},        {}, {0, 16.5}, {},
                                           {0, 16.5}, {}, {0, 49.5}, {}, {}, {0, 16.5}, {}, {}};

    CncDefaultValues    cnc_default_values{EMotion::RapidTraverse, // G0
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
    FileSplitting       file_splitting{FileSplittingType::work_motion_time, 0, 30, 0, "", false, ""};
    fanuc::FileSplitter fs{file_splitting, {ELanguage::Polish}};
    std::vector<std::vector<std::vector<fanuc::AttributeVariant>>> ret;
    fs.evaluate(cnc_default_values, std::move(data), {}, ret, time_result);
    verify(std::move(ret), std::move(expected_data));
}

} // namespace fanuc_test
