#include "stdafx.h"

#include <iostream>
#include <string>
#include <vector>

#include <boost/type_index.hpp>
#include <gtest/gtest.h>

#include "AttributesVisitor.h"
#include <fanuc/AllAttributesParser.h>

using namespace parser;

namespace fanuc_test {

class AllAttributesParserTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verify(std::string&& data, std::vector<fanuc::AttributeVariant>&& av_expected, bool ret_expected);
    void verify_remove_comment(std::string&& data, std::vector<fanuc::AttributeVariant>&& av_expected,
                               bool ret_expected);
    void verify_remove_optional_block(std::string&& data, std::vector<fanuc::AttributeVariant>&& av_expected,
                                      bool ret_expected);

    fanuc::word_map word_grammar_map;
};

const boost::none_t _{boost::none};

void AllAttributesParserTest::SetUp()
{
    word_grammar_map = {
        {"N", {"N", 1, 9999, 0, 0, false, fanuc::WordType::numbering}},
        {":", {":", 1, 9999, 0, 0, false, fanuc::WordType::numbering}},
        {"#", {"#", 1, 9999, 0, 0, false, fanuc::WordType::macro}},
        {"G", {"G", 1, 99, 1, 9, false, fanuc::WordType::code}},
        {"M", {"M", 1, 999, 0, 0, false, fanuc::WordType::code}},
        {"D", {"D", -999, 999, 0, 0, false, fanuc::WordType::misc}},
        {"F", {"F", 0, 999, 0, 9999, false, fanuc::WordType::feed}},
        {"T", {"T", 1, 999, 0, 0, false, fanuc::WordType::misc}},
        {"C", {"C", -999, 999, 0, 9999, true, fanuc::WordType::length}},
        {"X", {"X", -999, 999, 0, 9999, true, fanuc::WordType::length}},
        {"Y", {"Y", -999, 999, 0, 9999, true, fanuc::WordType::length}},
        {"Z", {"Z", -999, 999, 0, 9999, true, fanuc::WordType::length}},
        {"C2", {"C2", -999, 999, 0, 9999, true, fanuc::WordType::assignable}},
        {",C", {",C", -999, 999, 0, 9999, false, fanuc::WordType::misc}},
        {",R", {",R", -999, 999, 0, 9999, false, fanuc::WordType::misc}},
        {"=", {"=", -999, 999, 0, 9999, false, fanuc::WordType::calc}},
        {"+", {"+", -999, 999, 0, 9999, false, fanuc::WordType::calc}},
        {"-", {"-", -999, 999, 0, 9999, false, fanuc::WordType::calc}},
        {"*", {"*", -999, 999, 0, 9999, false, fanuc::WordType::calc}},
        {"/", {"/", -999, 999, 0, 9999, false, fanuc::WordType::calc}},
        {"GOTO", {"GOTO", 1, 9999, 0, 0, false, fanuc::WordType::misc}},
    };
}

void AllAttributesParserTest::verify(std::string&& data, std::vector<fanuc::AttributeVariant>&& av_expected,
                                     bool ret_expected)
{
    std::string message;

    std::vector<std::string> allowed_functions = {"SIN", "COS", "TAN",   "ATAN", "SQRT", "ABS", "ROUND", "FIX",
                                                  "FUP", "OR",  "XOR",   "AND",  "BIN",  "BCD", "OR",    "XOR",
                                                  "AND", "IF",  "WHILE", "DO",   "END",  "EQ",  "LE"};

    fanuc::FanucAttributeData  av;
    fanuc::AllAttributesParser ap{{word_grammar_map, word_grammar_map},
                                  std::move(allowed_functions),
                                  {},
                                  {},
                                  {false, false, false},
                                  {ELanguage::Polish},
                                  EFanucParserType::FanucMill,
                                  true};

    auto ret = ap.parse(1, data, av, message, true);

    if (ret)
    {
        EXPECT_TRUE(message.empty()) << data;
    }

    EXPECT_EQ(ret_expected, ret) << data;
    EXPECT_EQ(av_expected.size(), av.value.size()) << data;

    for (size_t x = 0; x < av.value.size(); ++x)
        EXPECT_TRUE(boost::apply_visitor(AttributesVisitor(av_expected[x]), av.value[x])) << "x:" << x << ", " << data;
}

void AllAttributesParserTest::verify_remove_comment(std::string&&                          data,
                                                    std::vector<fanuc::AttributeVariant>&& av_expected,
                                                    bool                                   ret_expected)
{
    std::string message;

    std::vector<std::string> allowed_functions = {"SIN", "COS", "TAN",   "ATAN", "SQRT", "ABS", "ROUND", "FIX",
                                                  "FUP", "OR",  "XOR",   "AND",  "BIN",  "BCD", "OR",    "XOR",
                                                  "AND", "IF",  "WHILE", "DO",   "END",  "EQ",  "LE"};

    fanuc::FanucAttributeData  av;
    fanuc::AllAttributesParser ap{{word_grammar_map, word_grammar_map},
                                  std::move(allowed_functions),
                                  {},
                                  {},
                                  {false, false, false},
                                  {ELanguage::Polish},
                                  EFanucParserType::FanucMill,
                                  true};

    auto ret = ap.remove_comment(1, data, av, message, true);

    if (ret)
    {
        EXPECT_TRUE(message.empty()) << data;
    }

    EXPECT_EQ(ret_expected, ret) << data;
    EXPECT_EQ(av_expected.size(), av.value.size()) << data;

    for (size_t x = 0; x < av.value.size(); ++x)
        EXPECT_TRUE(boost::apply_visitor(AttributesVisitor(av_expected[x]), av.value[x])) << "x:" << x << ", " << data;
}

void AllAttributesParserTest::verify_remove_optional_block(std::string&&                          data,
                                                           std::vector<fanuc::AttributeVariant>&& av_expected,
                                                           bool                                   ret_expected)
{
    std::string message;

    std::vector<std::string> allowed_functions = {"SIN", "COS", "TAN",   "ATAN", "SQRT", "ABS", "ROUND", "FIX",
                                                  "FUP", "OR",  "XOR",   "AND",  "BIN",  "BCD", "OR",    "XOR",
                                                  "AND", "IF",  "WHILE", "DO",   "END",  "EQ",  "LE"};

    fanuc::FanucAttributeData  av;
    fanuc::AllAttributesParser ap{{word_grammar_map, word_grammar_map},
                                  std::move(allowed_functions),
                                  {},
                                  {},
                                  {false, false, false},
                                  {ELanguage::Polish},
                                  EFanucParserType::FanucMill,
                                  true};

    auto ret = ap.remove_optional_block(1, data, av, message, true);

    if (ret)
    {
        EXPECT_TRUE(message.empty()) << data;
    }

    EXPECT_EQ(ret_expected, ret) << data;
    EXPECT_EQ(av_expected.size(), av.value.size()) << data;

    for (size_t x = 0; x < av.value.size(); ++x)
        EXPECT_TRUE(boost::apply_visitor(AttributesVisitor(av_expected[x]), av.value[x])) << "x:" << x << ", " << data;
}

TEST_F(AllAttributesParserTest, generalAttributes)
{
    verify("G10",
           {
               fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("10")},
           },
           true);

    verify(":100 G02",
           {fanuc::DecimalAttributeData{":", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
           true);

    verify("G2.2 X-100",
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")},
            fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("100")}},
           true);

    verify("G2.2 X-100.055",
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")},
            fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("100"), '.', std::string("055")}},
           true);

    verify("N100 G02 X100 Y200. Z300.55 M14",
           {fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("200"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("300"), '.', std::string("55")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("14")}},
           true);

    verify("N100G02X100Y200.Z300.55M14",
           {fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("200"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("300"), '.', std::string("55")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("14")}},
           true);

    verify("G2.2 -100", {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")}}, false);

    verify("G2.2 100", {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")}}, false);

    verify("G2.2 X-100 Z",
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")},
            fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("100")}},
           false);

    verify("M30F", {fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("30")}}, false);

    verify("M-30", {}, false);

    verify("X-100.", {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("100"), '.'}}, true);

    verify("X-100.55", {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("100"), '.', std::string("55")}},
           true);

    verify("X-100.55 4", {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("100"), '.', std::string("55")}},
           false);

    verify("X-100 55", {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("100")}}, false);

    verify("X-100. 55", {fanuc::DecimalAttributeData{"X", _, '-', _, _, std::string("100"), '.'}}, false);

    verify("T5 55", {fanuc::DecimalAttributeData{"T", _, _, _, _, std::string("5")}}, false);

    verify(",R5", {fanuc::DecimalAttributeData{",R", _, _, _, _, std::string("5")}}, true);

    verify(",C7", {fanuc::DecimalAttributeData{",C", _, _, _, _, std::string("7")}}, true);

    verify("A7", {}, false);

    verify("N100000", {}, false);

    verify("X-100000", {}, false);

    verify("X-100000.", {}, false);

    verify("X-100000.5364365", {}, false);

    verify("X-10.543634654", {}, false);

    verify("G2.2 X.1",
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, _, '.', std::string("1")}},
           true);

    verify("G2.2 X-.1",
           {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("2"), '.', std::string("2")},
            fanuc::DecimalAttributeData{"X", _, '-', _, _, _, '.', std::string("1")}},
           true);

    verify("D-4", {fanuc::DecimalAttributeData{"D", _, '-', _, _, std::string("4")}}, true);

    verify("D40", {fanuc::DecimalAttributeData{"D", _, _, _, _, std::string("40")}}, true);

    verify("F.1", {fanuc::DecimalAttributeData{"F", _, _, _, _, _, '.', std::string("1")}}, true);

    verify("/G02", {fanuc::DecimalAttributeData{"/"}, fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
           true);

    verify("/ G02", {fanuc::DecimalAttributeData{"/"}, fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
           true);

    verify("/1 G02",
           {fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("1")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
           true);

    verify("/1/2 G02",
           {fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("1")},
            fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("2")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
           true);

    verify("// G02", {}, false);

    verify("//2 G02", {}, false);

    verify(":100 , G02",
           {fanuc::DecimalAttributeData{":", _, _, _, _, std::string("100")}, fanuc::AttributeData<char>{",", 0},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
           true);

    verify(":100 ,G02", {fanuc::DecimalAttributeData{":", _, _, _, _, std::string("100")}}, false);

    verify(":100,G02", {fanuc::DecimalAttributeData{":", _, _, _, _, std::string("100")}}, false);

    verify(":100, G02",
           {fanuc::DecimalAttributeData{":", _, _, _, _, std::string("100")}, fanuc::AttributeData<char>{",", 0},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
           true);

    verify("#10=5",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("10")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("5")},
           },
           true);

    verify("#10=#20",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("10")},
               fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("20")},
           },
           true);

    verify("#10=#20+5",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("10")},
               fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("20")},
               fanuc::DecimalAttributeData{"+", _, _, _, _, std::string("5")},
           },
           true);

    verify("#10=#20/#30",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("10")},
               fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("20")},
               fanuc::DecimalAttributeData{"/", _, _, _, std::string("#"), std::string("30")},
           },
           true);

    verify("#10=#20-#30",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("10")},
               fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("20")},
               fanuc::DecimalAttributeData{"-", _, _, _, std::string("#"), std::string("30")},
           },
           true);

    verify("#10=#20-30",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("10")},
               fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("20")},
               fanuc::DecimalAttributeData{"-", _, _, _, _, std::string("30")},
           },
           true);

    verify("#10=6*7",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("10")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("6")},
               fanuc::DecimalAttributeData{"*", _, _, _, _, std::string("7")},
           },
           true);

    verify("#10=-5-#20",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("10")},
               fanuc::DecimalAttributeData{"=", _, '-', _, _, std::string("5")},
               fanuc::DecimalAttributeData{"-", _, _, _, std::string("#"), std::string("20")},
           },
           true);

    verify("G-#10",
           {
               fanuc::DecimalAttributeData{"G", _, '-', _, std::string("#"), std::string("10")},
           },
           true);

    verify("G#10",
           {
               fanuc::DecimalAttributeData{"G", _, _, _, std::string("#"), std::string("10")},
           },
           true);

    verify("M98 T-#1",
           {
               fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("98")},
               fanuc::DecimalAttributeData{"T", _, '-', _, std::string("#"), std::string("1")},
           },
           true);

    verify("N100T#101T#102M6(#6 CENTER DRILL)",
           {fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"T", _, _, _, std::string("#"), std::string("101")},
            fanuc::DecimalAttributeData{"T", _, _, _, std::string("#"), std::string("102")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("6")},
            fanuc::AttributeData<std::string>{"(", "#6 CENTER DRILL"}},
           true);

    verify("#[#1+#2-12]",
           {
               fanuc::DecimalAttributeData{"#", _, _, std::string("["), std::string("#"), std::string("1")},
               fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("2")},
               fanuc::DecimalAttributeData{"-", _, _, _, _, std::string("12"), _, _, std::string("]")},
           },
           true);

    verify("G01X[#1+#2]F#3",
           {
               fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("01")},
               fanuc::DecimalAttributeData{"X", _, _, std::string("["), std::string("#"), std::string("1")},
               fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("2"), _, _, std::string("]")},
               fanuc::DecimalAttributeData{"F", _, _, _, std::string("#"), std::string("3")},
           },
           true);

    verify("G01 X-[#1-#2] F#9",
           {
               fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("01")},
               fanuc::DecimalAttributeData{"X", _, '-', std::string("["), std::string("#"), std::string("1")},
               fanuc::DecimalAttributeData{"-", _, _, _, std::string("#"), std::string("2"), _, _, std::string("]")},
               fanuc::DecimalAttributeData{"F", _, _, _, std::string("#"), std::string("9")},
           },
           true);

    verify("X-[-#1+#2]",
           {
               fanuc::DecimalAttributeData{"X", _, '-', std::string("[")},
               fanuc::DecimalAttributeData{"-", _, _, _, std::string("#"), std::string("1")},
               fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("2"), _, _, std::string("]")},
           },
           true);

    verify("T-[-#1+#2]",
           {
               fanuc::DecimalAttributeData{"T", _, '-', std::string("[")},
               fanuc::DecimalAttributeData{"-", _, _, _, std::string("#"), std::string("1")},
               fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("2"), _, _, std::string("]")},
           },
           true);

    verify("#2=BIN[#1]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("2")},
               fanuc::DecimalAttributeData{"="},
               fanuc::DecimalAttributeData{"BIN", _, _, std::string("["), std::string("#"), std::string("1"), _, _,
                                           std::string("]")},
           },
           true);

    verify("#1=SIN[[[#2+#3]*#4+#5]*#6]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("1")},
               fanuc::DecimalAttributeData{"="},
               fanuc::DecimalAttributeData{"SIN", _, _, std::string("[[["), std::string("#"), std::string("2")},
               fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("3"), _, _, std::string("]")},
               fanuc::DecimalAttributeData{"*", _, _, _, std::string("#"), std::string("4")},
               fanuc::DecimalAttributeData{"+", _, _, _, std::string("#"), std::string("5"), _, _, std::string("]")},
               fanuc::DecimalAttributeData{"*", _, _, _, std::string("#"), std::string("6"), _, _, std::string("]")},
           },
           true);

    verify("#101=#100/2",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("101")},
               fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("100")},
               fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("2")},
           },
           true);

    verify("#113=8/2",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("8")},
               fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("2")},
           },
           true);

    verify("#113=8*2",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("8")},
               fanuc::DecimalAttributeData{"*", _, _, _, _, std::string("2")},
           },
           true);

    verify("#113=[8/2]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, std::string("["), _, std::string("8")},
               fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("2"), _, _, std::string("]")},
           },
           true);

    verify("#113=8/2]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("8")},
           },
           false);

    verify("#113=[8*2]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, std::string("["), _, std::string("8")},
               fanuc::DecimalAttributeData{"*", _, _, _, _, std::string("2"), _, _, std::string("]")},
           },
           true);

    verify("#113=8*2]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("8")},
           },
           false);

    verify("#113=[[8/2]]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, std::string("[["), _, std::string("8")},
               fanuc::DecimalAttributeData{"/", _, _, _, _, std::string("2"), _, _, std::string("]]")},
           },
           true);

    verify("#113=[8/2]]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, std::string("["), _, std::string("8")},
           },
           false);

    verify("#113=8/2]]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("8")},
           },
           false);

    verify("#113=[[8*2]]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, std::string("[["), _, std::string("8")},
               fanuc::DecimalAttributeData{"*", _, _, _, _, std::string("2"), _, _, std::string("]]")},
           },
           true);

    verify("#113=[8*2]]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, std::string("["), _, std::string("8")},
           },
           false);

    verify("#113=8*2]]",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("113")},
               fanuc::DecimalAttributeData{"=", _, _, _, _, std::string("8")},
           },
           false);

    verify("#3=#1 OR #2",
           {
               fanuc::DecimalAttributeData{"#", _, _, _, _, std::string("3")},
               fanuc::DecimalAttributeData{"=", _, _, _, std::string("#"), std::string("1")},
               fanuc::DecimalAttributeData{"OR", _, _, _, std::string("#"), std::string("2")},
           },
           true);

    verify("GOTO#2",
           {
               fanuc::DecimalAttributeData{"GOTO", _, _, _, std::string("#"), std::string("2")},
           },
           true);

    verify("GOTO2",
           {
               fanuc::DecimalAttributeData{"GOTO", _, _, _, _, std::string("2")},
           },
           true);

    verify("IF[#19EQ0]GOTO10",
           {
               fanuc::DecimalAttributeData{"IF", _, _, std::string("["), std::string("#"), std::string("19")},
               fanuc::DecimalAttributeData{"EQ", _, _, _, _, std::string("0"), _, _, std::string("]")},
               fanuc::DecimalAttributeData{"GOTO", _, _, _, _, std::string("10")},
           },
           true);

    verify("WHILE[#2 LE 10]DO 1",
           {
               fanuc::DecimalAttributeData{"WHILE", _, _, std::string("["), std::string("#"), std::string("2")},
               fanuc::DecimalAttributeData{"LE", _, _, _, _, std::string("10"), _, _, std::string("]")},
               fanuc::DecimalAttributeData{"DO", _, _, _, _, std::string("1")},
           },
           true);

    verify("END 1",
           {
               fanuc::DecimalAttributeData{"END", _, _, _, _, std::string("1")},
           },
           true);

    verify("M10.", {}, false);

    verify("#5=[#3-20]/5*SQRT[ABS[-#1+#2]]",
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
           true);

    verify("N100 G02 X100 Y200. Z300.55 M14 X200",
           {fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("100")},
            fanuc::DecimalAttributeData{"Y", _, _, _, _, std::string("200"), '.'},
            fanuc::DecimalAttributeData{"Z", _, _, _, _, std::string("300"), '.', std::string("55")},
            fanuc::DecimalAttributeData{"M", _, _, _, _, std::string("14")},
            fanuc::DecimalAttributeData{"X", _, _, _, _, std::string("200")}},
           false);

    verify("C2=5",
           {
               fanuc::DecimalAttributeData{"C2", '=', _, _, _, std::string("5")},
           },
           true);

    verify("C2 = 5",
           {
               fanuc::DecimalAttributeData{"C2", '=', _, _, _, std::string("5")},
           },
           true);

    verify("C25",
           {
               fanuc::DecimalAttributeData{"C", _, _, _, _, std::string("25")},
           },
           true);

    verify("G1 Z[-7.8-#26]",
           {
               fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("1")},
               fanuc::DecimalAttributeData{"Z", _, _, std::string("[")},
               fanuc::DecimalAttributeData{"-", _, _, _, _, std::string("7"), '.', std::string("8")},
               fanuc::DecimalAttributeData{"-", _, _, _, std::string("#"), std::string("26"), _, _, std::string("]")},
           },
           true);
}

TEST_F(AllAttributesParserTest, programStart)
{
    verify("%", {fanuc::AttributeData<char>{"%", 0}}, true);

    verify("%   ", {fanuc::AttributeData<char>{"%", 0}}, true);

    verify("%j", {}, false);

    verify("%5", {}, false);

    verify("% X100", {}, false);
}

TEST_F(AllAttributesParserTest, programName)
{
    verify("O1234", {fanuc::AttributeData<std::string>{"O", "1234"}}, true);

    verify("O1234     ", {fanuc::AttributeData<std::string>{"O", "1234"}}, true);

    verify("OA", {fanuc::AttributeData<std::string>{"O", "A"}}, true);

    verify("O12", {fanuc::AttributeData<std::string>{"O", "12"}}, true);

    verify("O1234.", {fanuc::AttributeData<std::string>{"O", "1234"}}, false);

    verify("O1234.5", {fanuc::AttributeData<std::string>{"O", "1234"}}, false);

    verify("O1234A", {fanuc::AttributeData<std::string>{"O", "1234A"}}, true);

    verify("O12.34", {fanuc::AttributeData<std::string>{"O", "12"}}, false);

    verify("O1234 A50", {fanuc::AttributeData<std::string>{"O", "1234"}}, false);

    verify("O1234 O1234", {fanuc::AttributeData<std::string>{"O", "1234"}}, false);

    verify("O0000", {fanuc::AttributeData<std::string>{"O", "0000"}}, true);

    verify("O00001", {fanuc::AttributeData<std::string>{"O", "00001"}}, true);

    verify("O44444", {fanuc::AttributeData<std::string>{"O", "44444"}}, true);

    verify("O1", {fanuc::AttributeData<std::string>{"O", "1"}}, true);

    verify("O", {}, false);

    verify("O1234(comment)",
           {fanuc::AttributeData<std::string>{"O", "1234"}, fanuc::AttributeData<std::string>{"(", "comment"}}, true);
}

TEST_F(AllAttributesParserTest, comment)
{
    verify("(This is my comment! )", {fanuc::AttributeData<std::string>{"(", "This is my comment! "}}, true);

    verify("()", {fanuc::AttributeData<std::string>{"(", ""}}, true);

    verify("(This is my comment! )    ", {fanuc::AttributeData<std::string>{"(", "This is my comment! "}}, true);

    verify("N50 ( Yet another comment)",
           {fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("50")},
            fanuc::AttributeData<std::string>{"(", " Yet another comment"}},
           true);

    verify("(This is my comment!",
           {

           },
           false);

    verify("This is my comment!)",
           {

           },
           false);

    verify(R"((1/2" DRILL))", {fanuc::AttributeData<std::string>{"(", R"(1/2" DRILL)"}}, true);
}

TEST_F(AllAttributesParserTest, removeComment)
{
    verify_remove_comment("(This is my comment! )",
                          {

                          },
                          true);

    verify_remove_comment("()",
                          {

                          },
                          true);

    verify_remove_comment("(This is my comment! )    ",
                          {

                          },
                          true);

    verify_remove_comment("N50 ( Yet another comment)",
                          {
                              fanuc::DecimalAttributeData{"N", _, _, _, _, std::string("50")},
                          },
                          true);

    verify_remove_comment("(This is my comment!",
                          {

                          },
                          false);

    verify_remove_comment("This is my comment!)",
                          {

                          },
                          false);

    verify_remove_comment(R"((1/2" DRILL))",
                          {

                          },
                          true);
}

TEST_F(AllAttributesParserTest, removeOptionalBlock)
{
    verify_remove_optional_block(
        "/G02", {fanuc::DecimalAttributeData{"/"}, fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
        true);

    verify_remove_optional_block(
        "/ G02", {fanuc::DecimalAttributeData{"/"}, fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}},
        true);

    verify_remove_optional_block("/] G02", {}, false);

    verify_remove_optional_block("/1 G02", {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}}, true);

    verify_remove_optional_block("/1] G02", {}, false);

    verify_remove_optional_block("/1/2 G02", {fanuc::DecimalAttributeData{"G", _, _, _, _, std::string("02")}}, true);

    verify_remove_optional_block("// G02", {}, false);

    verify_remove_optional_block("//2 G02", {}, false);
}

TEST_F(AllAttributesParserTest, emptyString)
{
    /*verify(" ",
    {
    }, true);*/
}

} // namespace fanuc_test
