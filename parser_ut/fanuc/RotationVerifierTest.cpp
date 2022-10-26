#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <GeneralParserDefines.h>
#include <fanuc/RotationVerifier.h>

using namespace parser;

namespace fanuc_test {

const boost::none_t _{boost::none};

class RotationVerifierTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verifyThrow(std::vector<fanuc::AttributeVariant>&& data, bool operator_turns_rotation, std::string&& msg);
    void verifyNoThrow(std::vector<fanuc::AttributeVariant>&& data, bool operator_turns_rotation, std::string&& msg);
};

void RotationVerifierTest::SetUp()
{
}

void RotationVerifierTest::verifyThrow(std::vector<fanuc::AttributeVariant>&& data, bool operator_turns_rotation,
                                       std::string&& msg)
{
    CncDefaultValues cnc{};
    cnc.operator_turns_on_rotation = operator_turns_rotation;
    EXPECT_THROW(fanuc::RotationVerifier()(cnc, data, {ELanguage::Polish}), fanuc::rotation_verifier_exception) << msg;
}

void RotationVerifierTest::verifyNoThrow(std::vector<fanuc::AttributeVariant>&& data, bool operator_turns_rotation,
                                         std::string&& msg)
{
    CncDefaultValues cnc{};
    cnc.operator_turns_on_rotation = operator_turns_rotation;
    EXPECT_NO_THROW(fanuc::RotationVerifier()(cnc, data, {ELanguage::Polish})) << msg;
}

TEST_F(RotationVerifierTest, Spindle)
{
    verifyNoThrow(
        {
            fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("10")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("03")},
        },
        false, "S10 M03 false");

    verifyThrow(
        {
            fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("10")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("03")},
        },
        true, "S10 M03 true");

    verifyThrow(
        {
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("03")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("81")},
        },
        false, "M03 G81 false");

    verifyThrow(
        {
            fanuc::DecimalAttributeData{"S", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("89")},
        },
        false, "S100 G89 false");

    verifyNoThrow(
        {
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("01")},
        },
        true, "G01 true");
}

} // namespace fanuc_test
