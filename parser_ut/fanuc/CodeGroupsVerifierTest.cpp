#include "stdafx.h"

#include <iostream>
#include <limits>
#include <string>

#include <boost/type_index.hpp>
#include <gtest/gtest.h>

#include <fanuc/CodeGroupsVerifier.h>

using namespace parser;

namespace fanuc_test {

const boost::none_t _{boost::none};

class CodeGroupsVerifierTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verify_throw(const fanuc::macro_map& macro_values, const fanuc::code_groups_map& gcode_groups,
                      const fanuc::code_groups_map& mcode_groups, int line, std::vector<fanuc::AttributeVariant>&& data,
                      std::string&& msg);

    void verify_no_throw(const fanuc::macro_map& macro_values, const fanuc::code_groups_map& gcode_groups,
                         const fanuc::code_groups_map& mcode_groups, int line,
                         std::vector<fanuc::AttributeVariant>&& data, std::string&& msg);

    fanuc::code_groups_map gcode_groups;
    fanuc::code_groups_map mcode_groups;
};

void CodeGroupsVerifierTest::SetUp()
{
    gcode_groups = {
        {{4, 0}, {"0"}}, {{5, 1}, {"0"}}, {{5, 4}, {"0"}}, {{2, 0}, {"1"}}, {{2, 2}, {"1"}},
    };
    mcode_groups = {
        {{14, 0}, {"0", "1"}},
        {{15, 0}, {"0"}},
        {{22, 0}, {"1"}},
        {{23, 0}, {"1"}},
    };
}

void CodeGroupsVerifierTest::verify_throw(const fanuc::macro_map&       macro_values,
                                          const fanuc::code_groups_map& gcode_groups,
                                          const fanuc::code_groups_map& mcode_groups, int line,
                                          std::vector<fanuc::AttributeVariant>&& data, std::string&& msg)
{
    EXPECT_THROW(fanuc::CodeGroupsVerifier()(macro_values, gcode_groups, mcode_groups, line, data, {ELanguage::Polish}),
                 fanuc::code_groups_exception)
        << msg;
}

void CodeGroupsVerifierTest::verify_no_throw(const fanuc::macro_map&       macro_values,
                                             const fanuc::code_groups_map& gcode_groups,
                                             const fanuc::code_groups_map& mcode_groups, int line,
                                             std::vector<fanuc::AttributeVariant>&& data, std::string&& msg)
{
    EXPECT_NO_THROW(
        fanuc::CodeGroupsVerifier()(macro_values, gcode_groups, mcode_groups, line, data, {ELanguage::Polish}))
        << msg;
}

TEST_F(CodeGroupsVerifierTest, GCodeGroup)
{
    verify_no_throw({}, gcode_groups, mcode_groups, 1,
                    {
                        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("04")},
                    },
                    "N10 G04");

    verify_no_throw({}, gcode_groups, mcode_groups, 1,
                    {
                        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("4")},
                        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")},
                    },
                    "N10 G4 G02");

    verify_throw({}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("4")},
                     fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("5"), '.', std::string("1")},
                 },
                 "N10 G4 G5.1");

    verify_throw({}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("10")},
                 },
                 "N10 G10");

    verify_no_throw({}, gcode_groups, mcode_groups, 1,
                    {
                        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                        fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("1")},
                        fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("5"), '.', std::string("4")},
                    },
                    "N10 #1=5.4");

    verify_throw({}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")},
                     fanuc::DecimalAttributeData{"G", _, _, _, std::string("#"), std::string("1")},
                 },
                 "N10 G2.2 G#1");

    verify_no_throw({{{1, 1}, 5.4}}, gcode_groups, mcode_groups, 1,
                    {
                        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")},
                        fanuc::DecimalAttributeData{"G", _, _, _, std::string("#"), std::string("1")},
                    },
                    "N10 G2.2 G#1");

    verify_throw({{{1, 1}, 5.4}}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("5"), '.', std::string("1")},
                     fanuc::DecimalAttributeData{"G", _, _, _, std::string("#"), std::string("1")},
                 },
                 "N10 G5.1 G#1");

    verify_throw({{{1, 1}, 5.4}}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("5"), '.', std::string("1")},
                     fanuc::DecimalAttributeData{"G", _, _, _, std::string("#"), std::string("2")},
                 },
                 "N10 G5.1 G#2");
}

TEST_F(CodeGroupsVerifierTest, MCodeGroup)
{
    verify_no_throw({}, gcode_groups, mcode_groups, 1,
                    {
                        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                        fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("14")},
                    },
                    "N10 M14");

    verify_no_throw({}, gcode_groups, mcode_groups, 1,
                    {
                        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                        fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("15")},
                        fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("22")},
                    },
                    "N10 M15 M22");

    verify_throw({}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("14")},
                     fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("15")},
                 },
                 "N10 M14 M15");

    verify_throw({}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("14")},
                     fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("23")},
                 },
                 "N10 M14 M23");

    verify_throw({}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("10")},
                 },
                 "N10 M10");

    verify_no_throw({}, gcode_groups, mcode_groups, 1,
                    {
                        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                        fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("3")},
                        fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("15")},
                    },
                    "N10 #3=15");

    verify_throw({}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("22")},
                     fanuc::DecimalAttributeData{"M", _, _, _, std::string("#"), std::string("3")},
                 },
                 "N10 M22 M#3");

    verify_no_throw({{{3, 1}, 15}}, gcode_groups, mcode_groups, 1,
                    {
                        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                        fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("22")},
                        fanuc::DecimalAttributeData{"M", _, _, _, std::string("#"), std::string("3")},
                    },
                    "N10 M22 M#3");

    verify_throw({{{3, 1}, 15}}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("14")},
                     fanuc::DecimalAttributeData{"M", _, _, _, std::string("#"), std::string("3")},
                 },
                 "N10 M14 M#3");

    verify_throw({{{3, 1}, 15}}, gcode_groups, mcode_groups, 1,
                 {
                     fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("10")},
                     fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("15")},
                     fanuc::DecimalAttributeData{"M", _, _, _, std::string("#"), std::string("2")},
                 },
                 "N10 M15 M#2");
}

} // namespace fanuc_test
