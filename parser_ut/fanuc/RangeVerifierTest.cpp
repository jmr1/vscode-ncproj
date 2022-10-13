#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <GeneralParserDefines.h>
#include <fanuc/RangeVerifier.h>

using namespace parser;

namespace fanuc_test {

const boost::none_t _{boost::none};

class RangeVerifierTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verifyThrow(std::vector<fanuc::AttributeVariant>&& data, std::string&& msg,
                     EMachineToolType machine_tool_type = EMachineToolType::Mill);
    void verifyNoThrow(std::vector<fanuc::AttributeVariant>&& data, std::string&& msg,
                       EMachineToolType machine_tool_type = EMachineToolType::Mill);
    void verifyThrow(fanuc::word_range_map&& data, std::string&& msg);
    void verifyNoThrow(fanuc::word_range_map&& data, std::string&& msg);

    Kinematics kinematics{};
    ZeroPoint  zero_point{};
};

void RangeVerifierTest::SetUp()
{
    kinematics = std::move(Kinematics{{{"X", {-500., 500.}}, {"Y", {-500., 500.}}, {"Z", {0., 1000.}}},
                                      20000,
                                      20000,
                                      10000,
                                      30,
                                      6,
                                      0,
                                      0,
                                      false,
                                      false});

    zero_point = std::move(ZeroPoint{-400, 0, 0});
}

void RangeVerifierTest::verifyThrow(std::vector<fanuc::AttributeVariant>&& data, std::string&& msg,
                                    EMachineToolType machine_tool_type /*= EMachineToolType::Mill*/)
{
    EXPECT_THROW(fanuc::RangeVerifier()(machine_tool_type, kinematics, data, {ELanguage::Polish}),
                 fanuc::range_verifier_exception)
        << msg;
}

void RangeVerifierTest::verifyNoThrow(std::vector<fanuc::AttributeVariant>&& data, std::string&& msg,
                                      EMachineToolType machine_tool_type /*= EMachineToolType::Mill*/)
{
    EXPECT_NO_THROW(fanuc::RangeVerifier()(machine_tool_type, kinematics, data, {ELanguage::Polish})) << msg;
}

void RangeVerifierTest::verifyThrow(fanuc::word_range_map&& data, std::string&& msg)
{
    EXPECT_THROW(fanuc::RangeVerifier()(kinematics, zero_point, data, {ELanguage::Polish}),
                 fanuc::range_verifier_exception)
        << msg;
}

void RangeVerifierTest::verifyNoThrow(fanuc::word_range_map&& data, std::string&& msg)
{
    EXPECT_NO_THROW(fanuc::RangeVerifier()(kinematics, zero_point, data, {ELanguage::Polish})) << msg;
}

TEST_F(RangeVerifierTest, Kinematics)
{
    verifyNoThrow(
        {
            fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("10")},
        },
        "S10");
    verifyThrow(
        {
            fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("10001")},
        },
        "S10001");
    verifyNoThrow(
        {
            fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("10")},
        },
        "T10");
    verifyThrow(
        {
            fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("31")},
        },
        "T31");
    verifyNoThrow(
        {
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("1000")},
        },
        "F1000");
    verifyThrow(
        {
            fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("31000")},
        },
        "F31000");

    verifyNoThrow(
        {
            fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("1020")},
        },
        "T1020", EMachineToolType::Lathe);
}

TEST_F(RangeVerifierTest, AxesRange)
{
    verifyNoThrow({{"X", {-10, 10, 30}}}, "Xok");
    verifyNoThrow({{"X", {-600, 10, 3000}}}, "Xok");
    verifyThrow({{"X", {-60, 1000, 3000}}}, "Xnotok");
    verifyNoThrow({{"X", {-60, 800, 3000}}}, "Xok");
}

} // namespace fanuc_test
