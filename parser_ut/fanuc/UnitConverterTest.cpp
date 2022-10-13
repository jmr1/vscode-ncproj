#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <fanuc/UnitConverter.h>

#include "AttributesVisitor.h"

namespace fanuc_test {

class UnitConverterTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verify(std::vector<fanuc::AttributeVariant>&& data, std::vector<fanuc::AttributeVariant>&& expected_data);

    fanuc::FanucWordGrammar               word_grammar;
    std::unique_ptr<fanuc::UnitConverter> underTest;
};

void UnitConverterTest::SetUp()
{
    word_grammar.metric   = {{"A", fanuc::WordGrammar{"A", -9999, 9999, 0, 9999, true, fanuc::WordType::angle}},
                           {"B", fanuc::WordGrammar{"B", -9999, 9999, 0, 9999, true, fanuc::WordType::angle}},
                           {"C", fanuc::WordGrammar{"C", -9999, 9999, 0, 9999, true, fanuc::WordType::angle}},
                           {"X", fanuc::WordGrammar{"X", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                           {"Y", fanuc::WordGrammar{"Y", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                           {"Z", fanuc::WordGrammar{"Z", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                           {"I", fanuc::WordGrammar{"X", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                           {"J", fanuc::WordGrammar{"Y", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                           {"K", fanuc::WordGrammar{"Z", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                           {"G", fanuc::WordGrammar{"G", 0, 999, 0, 9, false, fanuc::WordType::code}},
                           {"F", fanuc::WordGrammar{"F", 0, 9999, 0, 9999, false, fanuc::WordType::feed}}};
    word_grammar.imperial = {{"A", fanuc::WordGrammar{"A", -9999, 9999, 0, 9999, true, fanuc::WordType::angle}},
                             {"B", fanuc::WordGrammar{"B", -9999, 9999, 0, 9999, true, fanuc::WordType::angle}},
                             {"C", fanuc::WordGrammar{"C", -9999, 9999, 0, 9999, true, fanuc::WordType::angle}},
                             {"X", fanuc::WordGrammar{"X", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                             {"Y", fanuc::WordGrammar{"Y", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                             {"Z", fanuc::WordGrammar{"Z", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                             {"I", fanuc::WordGrammar{"X", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                             {"J", fanuc::WordGrammar{"Y", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                             {"K", fanuc::WordGrammar{"Z", -9999, 9999, 0, 9999, true, fanuc::WordType::length}},
                             {"G", fanuc::WordGrammar{"G", 0, 999, 0, 9, false, fanuc::WordType::code}},
                             {"F", fanuc::WordGrammar{"F", 0, 9999, 0, 9999, false, fanuc::WordType::feed}}};

    underTest.reset(new fanuc::UnitConverter(word_grammar));
}

void UnitConverterTest::verify(std::vector<fanuc::AttributeVariant>&& data,
                               std::vector<fanuc::AttributeVariant>&& expected_data)
{
    ASSERT_EQ(data.size(), expected_data.size());
    for (size_t x = 0; x < expected_data.size(); ++x)
        EXPECT_TRUE(boost::apply_visitor(AttributesVisitor(expected_data[x]), data[x])) << "Problem at index " << x;
}

TEST_F(UnitConverterTest, mm_to_inch)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"X", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("10"), '.',
                                    std::string("055")},
        fanuc::DecimalAttributeData{"A", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"F", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("033")},
        fanuc::DecimalAttributeData{"Z", boost::none, '-', boost::none, std::string("#"), std::string("10")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"X", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("3937")},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("3959")},
        fanuc::DecimalAttributeData{"A", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"F", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("0013")},
        fanuc::DecimalAttributeData{"Z", boost::none, '-', boost::none, std::string("#"), std::string("10")},
    };

    underTest->mm_to_inch(data);

    verify(std::move(data), std::move(expected_data));
}

TEST_F(UnitConverterTest, mm_to_inchG04)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("4")},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("10"), '.',
                                    std::string("055")},
        fanuc::DecimalAttributeData{"A", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"F", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("033")},
        fanuc::DecimalAttributeData{"Z", boost::none, '-', boost::none, std::string("#"), std::string("10")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("4")},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("10"), '.',
                                    std::string("055")},
        fanuc::DecimalAttributeData{"A", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"F", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("033")},
        fanuc::DecimalAttributeData{"Z", boost::none, '-', boost::none, std::string("#"), std::string("10")},
    };

    underTest->mm_to_inch(data);

    verify(std::move(data), std::move(expected_data));
}

TEST_F(UnitConverterTest, mm_to_inchLineComment)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"/"},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("10"), '.',
                                    std::string("055")},
        fanuc::DecimalAttributeData{"A", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"F", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("033")},
        fanuc::DecimalAttributeData{"Z", boost::none, '-', boost::none, std::string("#"), std::string("10")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"/"},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("10"), '.',
                                    std::string("055")},
        fanuc::DecimalAttributeData{"A", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"F", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("033")},
        fanuc::DecimalAttributeData{"Z", boost::none, '-', boost::none, std::string("#"), std::string("10")},
    };

    underTest->mm_to_inch(data);

    verify(std::move(data), std::move(expected_data));
}

TEST_F(UnitConverterTest, mm_to_inchG68)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("68")},
        fanuc::DecimalAttributeData{"X", boost::none, '-', boost::none, boost::none, std::string("165"), '.'},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("0")},
        fanuc::DecimalAttributeData{"Z", boost::none, boost::none, boost::none, boost::none, std::string("115"), '.'},
        fanuc::DecimalAttributeData{"I", boost::none, '-', boost::none, boost::none, std::string("90"), '.'},
        fanuc::DecimalAttributeData{"J", boost::none, boost::none, boost::none, boost::none, std::string("90"), '.'},
        fanuc::DecimalAttributeData{"K", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("0")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("68")},
        fanuc::DecimalAttributeData{"X", boost::none, '-', boost::none, boost::none, std::string("6"), '.',
                                    std::string("4961")},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("0")},
        fanuc::DecimalAttributeData{"Z", boost::none, boost::none, boost::none, boost::none, std::string("4"), '.',
                                    std::string("5276")},
        fanuc::DecimalAttributeData{"I", boost::none, '-', boost::none, boost::none, std::string("90"), '.'},
        fanuc::DecimalAttributeData{"J", boost::none, boost::none, boost::none, boost::none, std::string("90"), '.'},
        fanuc::DecimalAttributeData{"K", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("0")},
    };

    underTest->mm_to_inch(data);

    verify(std::move(data), std::move(expected_data));
}

TEST_F(UnitConverterTest, mm_to_inchG68_2)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("68"), '.',
                                    std::string("2")},
        fanuc::DecimalAttributeData{"X", boost::none, '-', boost::none, boost::none, std::string("165"), '.'},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("0")},
        fanuc::DecimalAttributeData{"Z", boost::none, boost::none, boost::none, boost::none, std::string("115"), '.'},
        fanuc::DecimalAttributeData{"I", boost::none, '-', boost::none, boost::none, std::string("90"), '.'},
        fanuc::DecimalAttributeData{"J", boost::none, boost::none, boost::none, boost::none, std::string("90"), '.'},
        fanuc::DecimalAttributeData{"K", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("0")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("68"), '.',
                                    std::string("2")},
        fanuc::DecimalAttributeData{"X", boost::none, '-', boost::none, boost::none, std::string("6"), '.',
                                    std::string("4961")},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("0")},
        fanuc::DecimalAttributeData{"Z", boost::none, boost::none, boost::none, boost::none, std::string("4"), '.',
                                    std::string("5276")},
        fanuc::DecimalAttributeData{"I", boost::none, '-', boost::none, boost::none, std::string("90"), '.'},
        fanuc::DecimalAttributeData{"J", boost::none, boost::none, boost::none, boost::none, std::string("90"), '.'},
        fanuc::DecimalAttributeData{"K", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("0")},
    };

    underTest->mm_to_inch(data);

    verify(std::move(data), std::move(expected_data));
}

TEST_F(UnitConverterTest, mm_to_inchG21)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("21")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("20")},
    };

    underTest->mm_to_inch(data);

    verify(std::move(data), std::move(expected_data));
}

TEST_F(UnitConverterTest, inch_to_mm)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"X", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("3937")},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("3959")},
        fanuc::DecimalAttributeData{"A", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"F", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("0013")},
        fanuc::DecimalAttributeData{"Z", boost::none, '-', boost::none, std::string("#"), std::string("10")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"X", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"Y", boost::none, boost::none, boost::none, boost::none, std::string("10"), '.',
                                    std::string("0559")},
        fanuc::DecimalAttributeData{"A", boost::none, boost::none, boost::none, boost::none, std::string("10")},
        fanuc::DecimalAttributeData{"F", boost::none, boost::none, boost::none, boost::none, std::string("0"), '.',
                                    std::string("033")},
        fanuc::DecimalAttributeData{"Z", boost::none, '-', boost::none, std::string("#"), std::string("10")},
    };

    underTest->inch_to_mm(data);

    verify(std::move(data), std::move(expected_data));
}

TEST_F(UnitConverterTest, inch_to_mmG20)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("20")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"G", boost::none, boost::none, boost::none, boost::none, std::string("21")},
    };

    underTest->inch_to_mm(data);

    verify(std::move(data), std::move(expected_data));
}

} // namespace fanuc_test