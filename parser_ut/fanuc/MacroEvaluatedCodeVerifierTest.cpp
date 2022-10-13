#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <fanuc/MacroEvaluatedCodeVerifier.h>

using namespace parser;

namespace fanuc_test {

const boost::none_t _{boost::none};

class MacroEvaluatedCodeVerifierTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verify_throw(const fanuc::macro_map& macro_values, int line, std::vector<fanuc::AttributeVariant>&& data,
                      std::string&& msg);

    void verify(const fanuc::macro_map& macro_values, int line, std::vector<fanuc::AttributeVariant>&& data,
                std::string&& expected, std::string&& msg);

    std::vector<std::string> allowed_operations;
};

void MacroEvaluatedCodeVerifierTest::SetUp()
{
    allowed_operations = {"SIN", "COS", "TAN", "ATAN", "SQRT", "ABS", "ROUND", "FIX", "FUP", "OR", "XOR", "AND",
                          "BIN", "BCD", "OR",  "XOR",  "AND",  "IF",  "WHILE", "DO",  "END", "EQ", "LE"};
}

void MacroEvaluatedCodeVerifierTest::verify_throw(const fanuc::macro_map& macro_values, int line,
                                                  std::vector<fanuc::AttributeVariant>&& data, std::string&& msg)
{
    EXPECT_THROW(fanuc::MacroEvaluatedCodeVerifier()(allowed_operations, macro_values, line, data, {ELanguage::Polish}),
                 fanuc::macro_evaluated_code_exception)
        << msg;
}

void MacroEvaluatedCodeVerifierTest::verify(const fanuc::macro_map& macro_values, int line,
                                            std::vector<fanuc::AttributeVariant>&& data, std::string&& expected,
                                            std::string&& msg)
{
    auto evaluated_code =
        fanuc::MacroEvaluatedCodeVerifier()(allowed_operations, macro_values, line, data, {ELanguage::Polish});
    EXPECT_EQ(expected, evaluated_code) << msg;
}

TEST_F(MacroEvaluatedCodeVerifierTest, GeneralVerifier)
{
    verify({}, 1,
           {
               fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("4")},
           },
           "", "T4");

    verify({}, 1,
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("1")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("5")},
           },
           "", "#1=5");

    verify({}, 1,
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("2")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("5"), '.', std::string("4")},
           },
           "", "#2=5.4");

    verify_throw({}, 1,
                 {
                     fanuc::DecimalAttributeData{"T", _, _, _, std::string("#"), std::string("1")},
                 },
                 "T#1");

    verify({{{1, 1}, 5}}, 1,
           {
               fanuc::DecimalAttributeData{"T", _, _, _, std::string("#"), std::string("1")},
           },
           "T5", "T#1");

    verify({{{1, 1}, 5}}, 1,
           {
               fanuc::DecimalAttributeData{"A", _, _, _, std::string("#"), std::string("1")},
           },
           "A5", "A#1");

    verify_throw({{{1, 1}, 5}}, 1,
                 {
                     fanuc::DecimalAttributeData{"A", _, _, _, std::string("#"), std::string("2")},
                 },
                 "A#2");
}

} // namespace fanuc_test
