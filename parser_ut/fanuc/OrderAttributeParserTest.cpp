#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <fanuc/OrderAttributeParser.h>

using namespace parser;

namespace fanuc_test {

class OrderAttributeParserTest : public ::testing::Test
{
protected:
    template <typename T1, typename T2>
    void execute(const std::string& data, char c1, char c2, bool ret_expected, std::tuple<T1, T2>&& tuple_expected)
    {
        std::string        message;
        std::tuple<T1, T2> td{};

        auto ret = underTest.parse(data, c1, c2, td, message);
        EXPECT_EQ(ret_expected, ret) << "Testing " << c1 << " vs " << c2 << ":" << message;
        EXPECT_EQ(std::get<0>(tuple_expected), std::get<0>(td)) << "Testing " << c1 << " vs " << c2 << ":" << message;
        EXPECT_EQ(std::get<1>(tuple_expected), std::get<1>(td)) << "Testing " << c1 << " vs " << c2 << ":" << message;
    }

    fanuc::OrderAttributeParser underTest;
};

TEST_F(OrderAttributeParserTest, orderAttribute)
{
    std::string data("N100 G02 G68 X100 V Y200 G72 Z300 M14");

    execute<float, float>(data, 'N', 'G', true, {100.f, 2.f});
    execute<float, float>(data, 'G', 'N', false, {2.f, 0.f});
}

} // namespace fanuc_test
