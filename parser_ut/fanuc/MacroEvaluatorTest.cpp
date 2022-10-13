#include "stdafx.h"

#include <limits>
#include <string>

#include <boost/type_index.hpp>
#include <gtest/gtest.h>

#include <fanuc/MacroEvaluator.h>

using namespace parser;

namespace fanuc_test {

const boost::none_t _{boost::none};

class MacroEvaluatorTest : public ::testing::Test
{
protected:
    template <bool ret_expected>
    void verify(fanuc::macro_map& macro_values, fanuc::macro_map&& expected_macro_values,
                std::vector<fanuc::AttributeVariant>&& data, std::string&& msg)
    {
        const double tolerance = 1e-8;
        std::string  message;

        fanuc::MacroEvaluator me;

        auto ret = me.evaluate(macro_values, 1, data, message, true, {ELanguage::Polish});
        EXPECT_EQ(ret_expected, ret) << msg;
        EXPECT_TRUE(message.empty()) << msg;
        EXPECT_NEAR(expected_macro_values.begin()->second, macro_values.at(expected_macro_values.begin()->first),
                    tolerance)
            << msg;
    }
};

template <>
void MacroEvaluatorTest::verify<false>(fanuc::macro_map& macro_values, fanuc::macro_map&& expected_macro_values,
                                       std::vector<fanuc::AttributeVariant>&& data, std::string&& msg)
{
    std::string message;

    fanuc::MacroEvaluator me;

    auto ret = me.evaluate(macro_values, 1, data, message, true, {ELanguage::Polish});
    EXPECT_EQ(false, ret) << msg;
    EXPECT_EQ(macro_values.count(expected_macro_values.begin()->first), std::size_t(0)) << msg;
}

TEST_F(MacroEvaluatorTest, generalEvaluator)
{
    fanuc::macro_map macro_values{};

    verify<true>(macro_values, {{{1, 1}, 9.}},
                 {
                     fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("1")},
                     fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("9")},
                 },
                 "#1=9");

    verify<true>(macro_values, {{{2, 1}, 5.}},
                 {
                     fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("2")},
                     fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("5")},
                 },
                 "#2=5");

    verify<true>(macro_values, {{{3, 1}, 45.}},
                 {
                     fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("3")},
                     fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("1")},
                     fanuc::DecimalAttributeData{"*", _, _, _, std::string("#"), std::string("2")},
                 },
                 "#3=#1*#2");

    verify<true>(macro_values, {{{4, 1}, 3.}},
                 {
                     fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("4")},
                     fanuc::DecimalAttributeData{"="},
                     fanuc::DecimalAttributeData{"SQRT", _, _, std::string("["), std::string("#"), std::string("1"), _,
                                                 std::string("]")},
                 },
                 "#4=SQRT[#1]");

    verify<false>(macro_values, {{{6, 0}, std::numeric_limits<double>::quiet_NaN()}},
                  {
                      fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("6")},
                      fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("2")},
                      fanuc::DecimalAttributeData{"/", _, _, _, std::string("#"), std::string("5")},
                  },
                  "#6=#2/#5");

    verify<true>(
        macro_values, {{{5, 1}, 10.}},
        {
            fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("5")},
            fanuc::DecimalAttributeData{"=", _, _, std::string("["), std::string("#"), std::string("3")},
            fanuc::DecimalAttributeData{"-", _, _, _, _, std::string("20"), _, _, std::string("]")},
            fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("5")},
            fanuc::DecimalAttributeData{"*"},
            fanuc::DecimalAttributeData{"SQRT", _, _, std::string("[")},
            fanuc::DecimalAttributeData{"ABS", _, _, std::string("[")},
            fanuc::DecimalAttributeData{"-", _, _, _, std::string("#"), std::string("1")},
            fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("2"), _, _, std::string("]]")},
        },
        "#5=[#3-20]/5*SQRT[ABS[-#1+#2]]");

    verify<true>(
        macro_values, {{{6, 1}, 0.965925826289068}},
        {
            fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("6")},
            fanuc::DecimalAttributeData{"="},
            fanuc::DecimalAttributeData{"SIN", _, _, std::string("[[["), std::string("#"), std::string("2")},
            fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("3"), _, _, std::string("]")},
            fanuc::DecimalAttributeData{"*", _, _, _, std::string("#"), std::string("4")},
            fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("1"), _, _, std::string("]")},
            fanuc::DecimalAttributeData{"*", _, _, _, std::string("#"), std::string("2"), _, _, std::string("]")},
        },
        "#6=SIN[[[#2+#3]*#4+#1]*#2]");

    verify<false>(macro_values, {{{7, 0}, std::numeric_limits<double>::quiet_NaN()}},
                  {
                      fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("7")},
                  },
                  "#7");

    verify<false>(macro_values, {{{7, 0}, std::numeric_limits<double>::quiet_NaN()}},
                  {
                      fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("7")},
                      fanuc::DecimalAttributeData{"="},
                  },
                  "#7=");

    verify<false>(macro_values, {{{7, 0}, std::numeric_limits<double>::quiet_NaN()}},
                  {
                      fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("7")},
                      fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("5")},
                      fanuc::DecimalAttributeData{"*"},
                  },
                  "#7=5*");

    verify<false>(macro_values, {{{7, 0}, std::numeric_limits<double>::quiet_NaN()}},
                  {
                      fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("7")},
                      fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("5")},
                      fanuc::DecimalAttributeData{"*"},
                  },
                  "#7=#5*");
}

} // namespace fanuc_test
