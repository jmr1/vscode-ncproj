#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <fanuc/Renumberer.h>

#include "AttributesVisitor.h"
#include <GeneralParserDefines.h>

namespace fanuc_test {

const boost::none_t _{boost::none};

class RenumbererTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verify(std::vector<fanuc::AttributeVariant>&& data, std::vector<fanuc::AttributeVariant>&& expected_data,
                std::string&& msg);

    template <typename T>
    void verify(fanuc::Renumberer& uc, RenumberSettings& rs, T&& in_number, T&& in)
    {
        std::vector<fanuc::AttributeVariant> data          = {in};
        std::vector<fanuc::AttributeVariant> expected_data = {in_number, in};

        uc.renumber(data, rs);

        verify(std::move(data), std::move(expected_data), in_number.word + *in_number.value);
    }

    template <typename T>
    void verify(fanuc::Renumberer& uc, RenumberSettings& rs, T&& in)
    {
        std::vector<fanuc::AttributeVariant> data          = {in};
        std::vector<fanuc::AttributeVariant> expected_data = {in};

        uc.renumber(data, rs);

        std::string msg;
        if constexpr (std::is_same_v<T, fanuc::AttributeData<std::string>> ||
                      std::is_same_v<T, fanuc::AttributeData<char>>)
            msg = in.word + in.value;
        else
            msg = in.word + *in.value;

        verify(std::move(data), std::move(expected_data), std::move(msg));
    }

    template <typename T>
    void verify2(fanuc::Renumberer& uc, RenumberSettings& rs, T&& in_number, T&& in,
                 fanuc::DecimalAttributeData existingN = {"N", _, _, _, _, std::string("666")})
    {
        std::vector<fanuc::AttributeVariant> data          = {existingN, in};
        std::vector<fanuc::AttributeVariant> expected_data = {in_number, in};

        uc.renumber(data, rs);

        verify(std::move(data), std::move(expected_data), in_number.word + *in_number.value);
    }

    template <typename T>
    void verify2(fanuc::Renumberer& uc, RenumberSettings& rs, T&& in)
    {
        std::vector<fanuc::AttributeVariant> data = {fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("666")},
                                                     in};
        std::vector<fanuc::AttributeVariant> expected_data = {in};

        uc.renumber(data, rs);

        std::string msg;
        if constexpr (std::is_same_v<T, fanuc::AttributeData<std::string>> ||
                      std::is_same_v<T, fanuc::AttributeData<char>>)
            msg = in.word + in.value;
        else
            msg = in.word + *in.value;

        verify(std::move(data), std::move(expected_data), std::move(msg));
    }

    std::unique_ptr<fanuc::Renumberer> underTest;
    fanuc::FanucWordGrammar            word_grammar;
};

void RenumbererTest::SetUp()
{
    word_grammar.metric   = {{"N", fanuc::WordGrammar{"N", 1, 9999, 0, 0, true, fanuc::WordType::numbering}},
                           {":", fanuc::WordGrammar{":", 1, 9999, 0, 0, true, fanuc::WordType::numbering}},
                           {"A", fanuc::WordGrammar{"A", -9999, 9999, 0, 9999, true, fanuc::WordType::angle}},
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
    word_grammar.imperial = {{"N", fanuc::WordGrammar{"N", 1, 9999, 0, 0, true, fanuc::WordType::numbering}},
                             {":", fanuc::WordGrammar{":", 1, 9999, 0, 0, true, fanuc::WordType::numbering}},
                             {"A", fanuc::WordGrammar{"A", -9999, 9999, 0, 9999, true, fanuc::WordType::angle}},
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

    underTest.reset(new fanuc::Renumberer(word_grammar));
}

void RenumbererTest::verify(std::vector<fanuc::AttributeVariant>&& data,
                            std::vector<fanuc::AttributeVariant>&& expected_data, std::string&& msg)
{
    ASSERT_EQ(data.size(), expected_data.size()) << msg;
    for (size_t x = 0; x < expected_data.size(); ++x)
        EXPECT_TRUE(boost::apply_visitor(AttributesVisitor(expected_data[x]), data[x]))
            << msg << " Problem at index " << x;
}

TEST_F(RenumbererTest, remove)
{
    std::vector<fanuc::AttributeVariant> data = {
        fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("5")},
        fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"A", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("0"), '.', std::string("033")},
        fanuc::DecimalAttributeData{"Z", _, '-', _, std::string("#"), std::string("10")},
    };

    std::vector<fanuc::AttributeVariant> expected_data = {
        fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("10"), '.', std::string("055")},
        fanuc::DecimalAttributeData{"A", _, _, _, _, std::string("10")},
        fanuc::DecimalAttributeData{"F", _, _, _, _, std::string("0"), '.', std::string("033")},
        fanuc::DecimalAttributeData{"Z", _, '-', _, std::string("#"), std::string("10")},
    };

    underTest->remove(data);

    verify(std::move(data), std::move(expected_data), "remove");
}

TEST_F(RenumbererTest, renumber)
{
    RenumberSettings rs{"N", 1, 1, 100, false, false};
    RenumberSettings rs2{"N", 2, 1, 5, false, true};
    RenumberSettings rs3{"N", 2, 3, 6, false, true};

    verify(*underTest, rs, fanuc::AttributeData<char>{"%"});

    verify(*underTest, rs, fanuc::AttributeData<std::string>{"O", "1234"});

    verify(*underTest, rs, fanuc::AttributeData<std::string>{"(", "The comment"});

    verify(*underTest, rs, fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("1")});

    verify(*underTest, rs, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("1")},
           fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("2")},
           fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs2, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0003")},
           fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs2, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0005")},
           fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs2, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0001")},
           fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs3, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0003")},
           fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs3, fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs3, fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs3, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0005")},
           fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    underTest->set_start_value(3);

    verify(*underTest, rs, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("3")},
           fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});
}

TEST_F(RenumbererTest, renumberExisting)
{
    RenumberSettings rs{"N", 1, 1, 100, true, false};
    RenumberSettings rs2{"N", 2, 1, 5, true, true};
    RenumberSettings rs3{"N", 2, 3, 6, true, true};

    verify(*underTest, rs, fanuc::AttributeData<char>{"%"});

    verify(*underTest, rs, fanuc::AttributeData<std::string>{"O", "1234"});

    verify(*underTest, rs, fanuc::AttributeData<std::string>{"(", "The comment"});

    verify(*underTest, rs, fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("1")});

    verify2(*underTest, rs, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("1")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify2(*underTest, rs, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify2(*underTest, rs2, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0003")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify2(*underTest, rs2, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0005")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify2(*underTest, rs2, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0001")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify2(*underTest, rs3, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0003")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify(*underTest, rs3, fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify2(*underTest, rs3, fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify2(*underTest, rs3, fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    verify2(*underTest, rs3, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("0005")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});

    underTest->set_start_value(3);

    verify2(*underTest, rs, fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("3")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("10")});
}

} // namespace fanuc_test
