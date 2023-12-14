#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <fanuc/SystemMacroAssigner.h>

using namespace parser;

namespace fanuc_test {

const boost::none_t _{boost::none};

class SystemMacroAssignerTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verify(std::vector<fanuc::AttributeVariant>&& data, std::string&& msg,
                fanuc::macro_map&& expected_macro_values);
};

void SystemMacroAssignerTest::SetUp()
{
}

void SystemMacroAssignerTest::verify(std::vector<fanuc::AttributeVariant>&& data, std::string&& msg,
                                     fanuc::macro_map&& expected_macro_values)
{
    fanuc::macro_map macro_values;
    fanuc::SystemMacroAssigner()(macro_values, 1, data);

    const double tolerance = 1e-8;
    EXPECT_EQ(expected_macro_values.size(), macro_values.size()) << msg;
    for (const auto& it : expected_macro_values)
    {
        EXPECT_TRUE(macro_values.count(it.first))
            << msg << " (id, line) = (" << it.first.id << ", " << it.first.line << ")";
        EXPECT_NEAR(it.second, macro_values.at(it.first), tolerance)
            << msg << " (id, line) = (" << it.first.id << ", " << it.first.line << ")";
    }
}

TEST_F(SystemMacroAssignerTest, Generic)
{
    verify({fanuc::DecimalAttributeData{"D", _, '-', _, _, std::string("4")}}, "D-4",
           {{{4107, 1}, -4}, {{4307, 1}, -4}, {{4507, 1}, -4}});

    verify(
        {
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("1000"), '.', std::string("43")},
        },
        "F1000.43", {{{4109, 1}, 1000.43}, {{4309, 1}, 1000.43}, {{4509, 1}, 1000.43}});

    verify(
        {
            fanuc::DecimalAttributeData{"F", _, _, _, _, _, '.', std::string("43")},
        },
        "F.43", {{{4109, 1}, .43}, {{4309, 1}, .43}, {{4509, 1}, .43}});

    verify(
        {
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("300")},
        },
        "M300", {{{4113, 1}, 300.}, {{4313, 1}, 300.}, {{4513, 1}, 300.}});

    verify(
        {
            fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("123")},
        },
        "N123", {{{4114, 1}, 123.}, {{4314, 1}, 123.}, {{4514, 1}, 123.}});

    // Macro value is double floating point but program name although composed of digits it is a string
    // verify(
    //     {
    //         fanuc::AttributeData<std::string>{"O", "1234"},
    //     },
    //     "O1234", {{{4115, 1}, 1234.}, {{4315, 1}, 1234.}, {{4515, 1}, 1234.}});

    verify(
        {
            fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("10")},
        },
        "S10", {{{4119, 1}, 10.}, {{4319, 1}, 10.}, {{4519, 1}, 10.}});
}

} // namespace fanuc_test
