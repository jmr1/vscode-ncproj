#include "stdafx.h"

#include <string>

#include <boost/type_index.hpp>
#include <gtest/gtest.h>

#include <fanuc/ValueCalculator.h>

using namespace parser;

namespace fanuc_test {

class ValueCalculatorTest : public ::testing::Test
{
protected:
    void verify(std::string&& data, const double& value_expected, fanuc::macro_map&& input_macro_values,
                bool ret_expected);
};

void ValueCalculatorTest::verify(std::string&& data, const double& value_expected,
                                 fanuc::macro_map&& input_macro_values, bool ret_expected)
{
    const double tolerance = 1e-8;
    std::string  message;
    double       value{};

    fanuc::ValueCalculator underTest;

    auto ret = underTest.parse(data, message, true, input_macro_values, value, {ELanguage::Polish});

    EXPECT_EQ(ret_expected, ret);
    EXPECT_TRUE(message.empty());
    EXPECT_NEAR(value_expected, value, tolerance);
}

TEST_F(ValueCalculatorTest, generalCalculator)
{
    verify("5+5", 10., {}, true);
    verify("SQRT(9)", 3., {}, true);
    verify("5+5*4", 25., {}, true);
    verify("(5+5)*4", 40., {}, true);
    verify("(-5.2+5.4)*4", 0.8, {}, true);
    verify("SIN(((-2+3)*4+5)*6)", 0.809016994374947, {}, true);
    verify("ABS[-6]/2", 3., {}, true);
    verify("#1+#2", 5., {{{1, 1}, 2.}, {{2, 1}, 3.}}, true);
    verify("#[#1+#2]", 7., {{{1, 1}, 2.}, {{2, 1}, 3.}, {{5, 1}, 7.}}, true);
    verify("8/2", 4., {}, true);
    verify("[8/2]", 4., {}, true);
    verify("[[8/2]]", 4., {}, true);
}

} // namespace fanuc_test
