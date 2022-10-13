#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include "AttributesVisitor.h"
#include <GeneralParserDefines.h>
#include <fanuc/AxesRotator.h>

namespace fanuc_test {

const boost::none_t _{boost::none};

class AxesRotatorTest : public ::testing::Test
{
protected:
    void verify(std::vector<fanuc::AttributeVariant>&& data, std::vector<fanuc::AttributeVariant>&& expected_data)
    {
        ASSERT_EQ(data.size(), expected_data.size());
        for (size_t x = 0; x < expected_data.size(); ++x)
            EXPECT_TRUE(boost::apply_visitor(AttributesVisitor(expected_data[x]), data[x])) << "Problem at index " << x;
    }
};

TEST_F(AxesRotatorTest, Xrotate90degrees)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("10")},
        fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"Z", _, _, _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"Z", _, '-', _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("18")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("19")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
        fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("10")},
        fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"J", _, '-', _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"K", _, _, _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"K", _, '-', _, std::string("#"), std::string("10")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("10")},
        fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"Y", _, _, _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"Y", _, '-', _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("18")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("19")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
        fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("10")},
        fanuc::DecimalAttributeData{"K", _, '-', _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"J", _, _, _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"J", _, '-', _, std::string("#"), std::string("10")},
    };

    fanuc::AxesRotator()(AxesRotatingOption::Xrotate90degrees, data);

    verify(std::move(data), std::move(expected_data));
}

TEST_F(AxesRotatorTest, Xrotate270degrees)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("10")},
        fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"Y", _, '-', _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"Z", _, _, _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"Z", _, '-', _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("18")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("19")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
        fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("10")},
        fanuc::DecimalAttributeData{"J", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"J", _, '-', _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"K", _, _, _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"K", _, '-', _, std::string("#"), std::string("10")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("10")},
        fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"Z", _, '-', _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"Y", _, '-', _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"Y", _, _, _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("18")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("17")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("19")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("3")},
        fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2")},
        fanuc::DecimalAttributeData{"I", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"I", _, '-', _, _, std::string("10")},
        fanuc::DecimalAttributeData{"K", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"K", _, '-', _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"J", _, '-', _, std::string("#"), std::string("10")},
        fanuc::DecimalAttributeData{"J", _, _, _, std::string("#"), std::string("10")},
    };

    fanuc::AxesRotator()(AxesRotatingOption::Xrotate270degrees, data);

    verify(std::move(data), std::move(expected_data));
}

} // namespace fanuc_test
