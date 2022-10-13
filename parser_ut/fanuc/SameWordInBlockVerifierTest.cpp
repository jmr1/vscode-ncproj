#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <GeneralParserDefines.h>
#include <fanuc/SameWordInBlockVerifier.h>

using namespace parser;

namespace fanuc_test {

const boost::none_t _{boost::none};

class SameWordInBlockVerifierTest : public ::testing::Test
{
protected:
    std::vector<fanuc::AttributeVariant> data;
};

TEST_F(SameWordInBlockVerifierTest, exceptionThrownWhenDuplicatedWords)
{
    data = {
        fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("20")},
    };

    EXPECT_THROW(fanuc::SameWordInBlockVerifier()("M", data, {ELanguage::Polish}), fanuc::same_word_verifier_exception)
        << "M10 M20";
}

TEST_F(SameWordInBlockVerifierTest, exceptionNotThrownWhenNoDuplicatedWords)
{
    data = {
        fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")},
    };

    EXPECT_NO_THROW(fanuc::SameWordInBlockVerifier()("M", data, {ELanguage::Polish})) << "M10 X10";
}

} // namespace fanuc_test
