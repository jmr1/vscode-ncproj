#include "stdafx.h"

#include <iostream>
#include <string>
#include <vector>

#include <boost/type_index.hpp>
#include <gtest/gtest.h>

#include "AttributesVisitor.h"
#include <heidenhain/AllAttributesParser.h>

using namespace parser;

namespace heidenhain_test {

class AllAttributesParserHHTest : public ::testing::Test
{
protected:
    void SetUp() override;

    void verify(std::string&& data, std::vector<heidenhain::AttributeVariant>&& av_expected, bool ret_expected);
};

const boost::none_t _{boost::none};

void AllAttributesParserHHTest::SetUp()
{
    /*const std::vector<std::string> allowed_functions = {
     "#","%",
     "A","ABST","AN","ANGLE","APPR","AXIAL","AXIS",
     "B","BACK","BEGIN","BLANK","BLK","BX","BY","BZ",
     "C","CALL","CC","CCA","CCPA","CCPR","CCX","CCY","CCZ","CDATA","CHF","CIRC","CLSD","CONT","CONTOUR","COORD","CP","CR","CT","CTP","CYCL",
     "D","DATUM","DE", "DEF","DEP","DEPTH","DIST","DL","DO","DP","DR","DWELL","DX","DY","DZ",
     "E","EN","END","ENDWHILE","ERROR","EULER","EULNU","EULPR","EULROT",
     "F","FC","FCT","FL","FLT","FMAX","FN","FORM","FPOL","FSELECT","FUNCTION",
     "G","GOTO","GS",
     "H","HSC-MODE:",
     "I","IA","IAN","IB","IC","ICCX","ICCY","ID","IDX","IF","INCH","IPA","IPR","IROT","IX","IY","IZ",
     "J",
     "K","K1X","K1Y","K1Z","K2X","K2Y","K2Z","K3X","K3Y","K3Z","KONTURLABEL",
     "L","LA","LABEL","LBL_STRING","LBL_VALUE","LCT","LEN","LN","LP","LT",
     "M","MAX","MB","MM","MOVE",
     "N","NR","NX","NY","NZ",
     "O","OHNE",
     "P","P01","P02","P03","P04","P05","P06","P07","P1X","P1Y","PA","PAR","PASSE","PAT","PATTERN","PATHCTRL","PCT","PDX","PDY","PECKG","PGM","PITCH","PITCHCIRC","PLANE","PLCT","PLN","PLNG","PLNGNG","PLT","POINTS","POS","PR","PROBE","PROF.","PROJECTED",
     "R","RADIUS","RAN","RAYON","RCCX","RCCY","RELATIV","REP","RESET","RL","RND","ROT","ROTX","ROTY","ROTZ","ROW","RPR","RR","RX",
     "S","SCL","SEL","SEQ","SET","SPA","SPATIAL","SPB","SPC","SPL","STAY","START","STEIG","STEP","STOP","SYSREAD","SYSWRITE",
     "T","TA","TABLE","TAP","TCH","TCP","TCPM","TEMP.","TIEFE","TOOL","TRANS","TURN","TX","TY","TZ",
     "U","UNBLANK","UP",
     "V","V.ZEIT","VECTOR","VZEIT",
     "W","WHILE","WINKEL",
     "X",
     "Y",
     "Z","ZEIT","ZUSTLG"
 };*/
}

void AllAttributesParserHHTest::verify(std::string&& data, std::vector<heidenhain::AttributeVariant>&& av_expected,
                                       bool ret_expected)
{
    std::string message;

    heidenhain::HeidenhainAttributeData av;
    heidenhain::AllAttributesParser     ap{{false, false, false}, {ELanguage::Polish}};

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

TEST_F(AllAttributesParserHHTest, comment)
{
    verify(";This is my comment! ", {heidenhain::CommentData{';', "This is my comment! ", 0}}, true);

    verify("; This is my comment! ", {heidenhain::CommentData{';', " This is my comment! ", 0}}, true);

    verify(";This is my comment! ~", {heidenhain::CommentData{';', "This is my comment! ", '~'}}, true);

    verify(";", {heidenhain::CommentData{';', "", 0}}, true);

    verify(";~", {heidenhain::CommentData{';', "", '~'}}, true);

    verify(";This is~ my comment!", {}, false);

    verify(";~This is my comment!", {}, false);

    verify(";This is my comment! ~~", {}, false);

    verify("44 ;This is my comment! ",
           {heidenhain::LineNumberData{44}, heidenhain::CommentData{';', "This is my comment! ", 0}}, true);

    verify("44;This is my comment! ", {}, false);
}

TEST_F(AllAttributesParserHHTest, programBeginEnd)
{
    verify("BEGIN PGM hei530_origins MM",
           {heidenhain::ProgramBeginEndData{heidenhain::EBeginEnd::begin, "hei530_origins", heidenhain::EUnit::mm}},
           true);

    verify(" BEGIN   PGM    hei530_origins    MM   ",
           {heidenhain::ProgramBeginEndData{heidenhain::EBeginEnd::begin, "hei530_origins", heidenhain::EUnit::mm}},
           true);

    verify("BEGINPGM hei530_origins MM", {}, false);

    verify("BEGIN PGMhei530_origins MM", {}, false);

    verify("BEGIN PGM hei530_originsMM", {}, false);

    verify("0 BEGIN PGM hei530_origins MM",
           {heidenhain::LineNumberData{0},
            heidenhain::ProgramBeginEndData{heidenhain::EBeginEnd::begin, "hei530_origins", heidenhain::EUnit::mm}},
           true);

    verify("0    BEGIN PGM hei530_origins MM",
           {heidenhain::LineNumberData{0},
            heidenhain::ProgramBeginEndData{heidenhain::EBeginEnd::begin, "hei530_origins", heidenhain::EUnit::mm}},
           true);

    verify("0\tBEGIN PGM hei530_origins MM",
           {heidenhain::LineNumberData{0},
            heidenhain::ProgramBeginEndData{heidenhain::EBeginEnd::begin, "hei530_origins", heidenhain::EUnit::mm}},
           true);

    verify("0BEGIN PGM hei530_origins MM", {}, false);

    verify("BEGIN hei530_origins MM", {}, false);

    verify("BEGIN PGMbug hei530_origins MM", {}, false);

    verify("BEGINbug PGM hei530_origins MM", {}, false);

    verify("3 BEGIN bug PGM hei530_origins MM",
           {
               heidenhain::LineNumberData{3},
           },
           false);

    verify("BEGIN PGM hei530 origins MM", {}, false);

    verify("BEGIN PGM hei530_origins MMbug", {}, false);

    verify("BEGIN PGM hei530_origins MM bug", {}, false);

    verify("END PGM hei530_origins INCH",
           {heidenhain::ProgramBeginEndData{heidenhain::EBeginEnd::end, "hei530_origins", heidenhain::EUnit::inch}},
           true);

    verify("8 END PGM hei530_origins INCH",
           {heidenhain::LineNumberData{8},
            heidenhain::ProgramBeginEndData{heidenhain::EBeginEnd::end, "hei530_origins", heidenhain::EUnit::inch}},
           true);

    verify("-8 END PGM hei530_origins INCH", {}, false);

    verify("8. END PGM hei530_origins INCH", {}, false);

    verify("8.2 END PGM hei530_origins INCH", {}, false);

    verify("8,2 END PGM hei530_origins INCH", {}, false);

    verify("0 BEGIN PGM hei530_origins MM ;This is my comment! ",
           {heidenhain::LineNumberData{0},
            heidenhain::ProgramBeginEndData{heidenhain::EBeginEnd::begin, "hei530_origins", heidenhain::EUnit::mm},
            heidenhain::CommentData{';', "This is my comment! ", 0}},
           true);

    verify("0 BEGIN PGM hei530_origins MM;This is my comment! ",
           {
               heidenhain::LineNumberData{0},
           },
           false);
}

TEST_F(AllAttributesParserHHTest, generalAttribute)
{
    verify("1 BLK FORM 0.1 Z X+0 Y+0 Z-50",
           {
               heidenhain::LineNumberData{1},
               heidenhain::GeneralAttributeData{"BLK"},
               heidenhain::GeneralAttributeData{"FORM", _, _, std::string("0"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"Z"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("0")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("0")},
               heidenhain::GeneralAttributeData{"Z", '-', _, std::string("50")},
           },
           true);

    verify("2 BLK FORM 0.2 X+100 Y+100 Z+0",
           {
               heidenhain::LineNumberData{2},
               heidenhain::GeneralAttributeData{"BLK"},
               heidenhain::GeneralAttributeData{"FORM", _, _, std::string("0"), '.', std::string("2")},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("100")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("100")},
               heidenhain::GeneralAttributeData{"Z", '+', _, std::string("0")},
           },
           true);

    verify("3 TOOL DEF 6 L+7.5 R+3",
           {
               heidenhain::LineNumberData{3},
               heidenhain::GeneralAttributeData{"TOOL"},
               heidenhain::GeneralAttributeData{"DEF", _, _, std::string("6")},
               heidenhain::GeneralAttributeData{"L", '+', _, std::string("7"), '.', std::string("5")},
               heidenhain::GeneralAttributeData{"R", '+', _, std::string("3")},
           },
           true);

    verify("4 TOOL CALL 6 Z S2000 F650 DL+1 DR+0.5",
           {
               heidenhain::LineNumberData{4},
               heidenhain::GeneralAttributeData{"TOOL"},
               heidenhain::GeneralAttributeData{"CALL", _, _, std::string("6")},
               heidenhain::GeneralAttributeData{"Z"},
               heidenhain::GeneralAttributeData{"S", _, _, std::string("2000")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("650")},
               heidenhain::GeneralAttributeData{"DL", '+', _, std::string("1")},
               heidenhain::GeneralAttributeData{"DR", '+', _, std::string("0"), '.', std::string("5")},
           },
           true);

    verify("5 L Z+100 R0 FMAX",
           {
               heidenhain::LineNumberData{5},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"Z", '+', _, std::string("100")},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("0")},
               heidenhain::GeneralAttributeData{"FMAX"},
           },
           true);

    verify("6 L X-10 Y-10 R0 FMAX M6",
           {
               heidenhain::LineNumberData{6},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"X", '-', _, std::string("10")},
               heidenhain::GeneralAttributeData{"Y", '-', _, std::string("10")},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("0")},
               heidenhain::GeneralAttributeData{"FMAX"},
               heidenhain::GeneralAttributeData{"M", _, _, std::string("6")},
           },
           true);

    verify("8 APPR LT X+20 Y+20 LEN 15 RR F100",
           {
               heidenhain::LineNumberData{8},
               heidenhain::GeneralAttributeData{"APPR"},
               heidenhain::GeneralAttributeData{"LT"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"LEN", _, _, std::string("15")},
               heidenhain::GeneralAttributeData{"RR"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
           },
           true);

    verify("9 L X+35 Y+35",
           {
               heidenhain::LineNumberData{9},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("35")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("35")},
           },
           true);

    verify("8 APPR LN X+10 Y+20 LEN 15 RR F100",
           {
               heidenhain::LineNumberData{8},
               heidenhain::GeneralAttributeData{"APPR"},
               heidenhain::GeneralAttributeData{"LN"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("10")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"LEN", _, _, std::string("15")},
               heidenhain::GeneralAttributeData{"RR"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
           },
           true);

    verify("8 APPR CT X+10 Y+20 CCA 180 R10 RR F100",
           {
               heidenhain::LineNumberData{8},
               heidenhain::GeneralAttributeData{"APPR"},
               heidenhain::GeneralAttributeData{"CT"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("10")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"CCA", _, _, std::string("180")},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("10")},
               heidenhain::GeneralAttributeData{"RR"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
           },
           true);

    verify("8 APPR LCT X+10 Y+20 R10 RR F100",
           {
               heidenhain::LineNumberData{8},
               heidenhain::GeneralAttributeData{"APPR"},
               heidenhain::GeneralAttributeData{"LCT"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("10")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("10")},
               heidenhain::GeneralAttributeData{"RR"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
           },
           true);

    verify("23 L X+30 Y+35 RR F100",
           {
               heidenhain::LineNumberData{23},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("30")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("35")},
               heidenhain::GeneralAttributeData{"RR"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
           },
           true);

    verify("24 L Y+20 RR F100",
           {
               heidenhain::LineNumberData{24},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"RR"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
           },
           true);

    verify("25 DEP LT LEN 12.5 F100 M2",
           {
               heidenhain::LineNumberData{25},
               heidenhain::GeneralAttributeData{"DEP"},
               heidenhain::GeneralAttributeData{"LT"},
               heidenhain::GeneralAttributeData{"LEN", _, _, std::string("12"), '.', std::string("5")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
               heidenhain::GeneralAttributeData{"M", _, _, std::string("2")},
           },
           true);

    verify("25 DEP LN LEN+20 F100 M2",
           {
               heidenhain::LineNumberData{25},
               heidenhain::GeneralAttributeData{"DEP"},
               heidenhain::GeneralAttributeData{"LN"},
               heidenhain::GeneralAttributeData{"LEN", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
               heidenhain::GeneralAttributeData{"M", _, _, std::string("2")},
           },
           true);

    verify("25 DEP CT CCA 180 R+8 F100 M2",
           {
               heidenhain::LineNumberData{25},
               heidenhain::GeneralAttributeData{"DEP"},
               heidenhain::GeneralAttributeData{"CT"},
               heidenhain::GeneralAttributeData{"CCA", _, _, std::string("180")},
               heidenhain::GeneralAttributeData{"R", '+', _, std::string("8")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
               heidenhain::GeneralAttributeData{"M", _, _, std::string("2")},
           },
           true);

    verify("25 DEP LCT X+10 Y+12 R8 F100 M2",
           {
               heidenhain::LineNumberData{25},
               heidenhain::GeneralAttributeData{"DEP"},
               heidenhain::GeneralAttributeData{"LCT"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("10")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("12")},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("8")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
               heidenhain::GeneralAttributeData{"M", _, _, std::string("2")},
           },
           true);

    verify("7 L X+10 Y+40 RL F200 M3",
           {
               heidenhain::LineNumberData{7},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("10")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("40")},
               heidenhain::GeneralAttributeData{"RL"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("200")},
               heidenhain::GeneralAttributeData{"M", _, _, std::string("3")},
           },
           true);

    verify("8 L IX+20 IY-15",
           {
               heidenhain::LineNumberData{8},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"IX", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"IY", '-', _, std::string("15")},
           },
           true);

    verify("9 L X+60 IY-10",
           {
               heidenhain::LineNumberData{9},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("60")},
               heidenhain::GeneralAttributeData{"IY", '-', _, std::string("10")},
           },
           true);

    verify("12 CC X+45 Y+25",
           {
               heidenhain::LineNumberData{12},
               heidenhain::GeneralAttributeData{"CC"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("45")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("25")},
           },
           true);

    verify("13 LP PR+30 PA+0 RR F300 M3",
           {
               heidenhain::LineNumberData{13},
               heidenhain::GeneralAttributeData{"LP"},
               heidenhain::GeneralAttributeData{"PR", '+', _, std::string("30")},
               heidenhain::GeneralAttributeData{"PA", '+', _, std::string("0")},
               heidenhain::GeneralAttributeData{"RR"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("300")},
               heidenhain::GeneralAttributeData{"M", _, _, std::string("3")},
           },
           true);

    verify("14 LP PA+60",
           {
               heidenhain::LineNumberData{14},
               heidenhain::GeneralAttributeData{"LP"},
               heidenhain::GeneralAttributeData{"PA", '+', _, std::string("60")},
           },
           true);

    verify("15 LP IPA+60",
           {
               heidenhain::LineNumberData{15},
               heidenhain::GeneralAttributeData{"LP"},
               heidenhain::GeneralAttributeData{"IPA", '+', _, std::string("60")},
           },
           true);

    verify("16 LP PA+180",
           {
               heidenhain::LineNumberData{16},
               heidenhain::GeneralAttributeData{"LP"},
               heidenhain::GeneralAttributeData{"PA", '+', _, std::string("180")},
           },
           true);

    verify("9 CHF 12 F250",
           {
               heidenhain::LineNumberData{9},
               heidenhain::GeneralAttributeData{"CHF", _, _, std::string("12")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("250")},
           },
           true);

    verify("7 RND R5 F100",
           {
               heidenhain::LineNumberData{7},
               heidenhain::GeneralAttributeData{"RND"},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("5")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
           },
           true);

    verify("7 C X+45 Y+25 DR+",
           {
               heidenhain::LineNumberData{7},
               heidenhain::GeneralAttributeData{"C"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("45")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("25")},
               heidenhain::GeneralAttributeData{"DR", '+'},
           },
           true);

    verify("20 CP PA+180 DR+",
           {
               heidenhain::LineNumberData{20},
               heidenhain::GeneralAttributeData{"CP"},
               heidenhain::GeneralAttributeData{"PA", '+', _, std::string("180")},
               heidenhain::GeneralAttributeData{"DR", '+'},
           },
           true);

    verify("11 CR X+70 Y+40 R+20 DR-",
           {
               heidenhain::LineNumberData{11},
               heidenhain::GeneralAttributeData{"CR"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("70")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("40")},
               heidenhain::GeneralAttributeData{"R", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"DR", '-'},
           },
           true);

    verify("15 CTP PR+30 PA+30",
           {
               heidenhain::LineNumberData{15},
               heidenhain::GeneralAttributeData{"CTP"},
               heidenhain::GeneralAttributeData{"PR", '+', _, std::string("30")},
               heidenhain::GeneralAttributeData{"PA", '+', _, std::string("30")},
           },
           true);

    verify("15 CP IPA-1800 IZ+5 DR- RL F50",
           {
               heidenhain::LineNumberData{15},
               heidenhain::GeneralAttributeData{"CP"},
               heidenhain::GeneralAttributeData{"IPA", '-', _, std::string("1800")},
               heidenhain::GeneralAttributeData{"IZ", '+', _, std::string("5")},
               heidenhain::GeneralAttributeData{"DR", '-'},
               heidenhain::GeneralAttributeData{"RL"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("50")},
           },
           true);

    verify("7 FPOL X+20 Y+30",
           {
               heidenhain::LineNumberData{7},
               heidenhain::GeneralAttributeData{"FPOL"},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("30")},
           },
           true);

    verify("8 FL IX+10 Y+20 RR F100",
           {
               heidenhain::LineNumberData{8},
               heidenhain::GeneralAttributeData{"FL"},
               heidenhain::GeneralAttributeData{"IX", '+', _, std::string("10")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"RR"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
           },
           true);

    verify("9 FCT PR+15 IPA+30 DR+ R15",
           {
               heidenhain::LineNumberData{9},
               heidenhain::GeneralAttributeData{"FCT"},
               heidenhain::GeneralAttributeData{"PR", '+', _, std::string("15")},
               heidenhain::GeneralAttributeData{"IPA", '+', _, std::string("30")},
               heidenhain::GeneralAttributeData{"DR", '+'},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("15")},
           },
           true);

    verify("10 FC CCX+20 CCY+15 DR+ R15",
           {
               heidenhain::LineNumberData{10},
               heidenhain::GeneralAttributeData{"FC"},
               heidenhain::GeneralAttributeData{"CCX", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"CCY", '+', _, std::string("15")},
               heidenhain::GeneralAttributeData{"DR", '+'},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("15")},
           },
           true);

    verify("13 FC DR+ R15 CCPR+35 CCPA+40",
           {
               heidenhain::LineNumberData{13},
               heidenhain::GeneralAttributeData{"FC"},
               heidenhain::GeneralAttributeData{"DR", '+'},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("15")},
               heidenhain::GeneralAttributeData{"CCPR", '+', _, std::string("35")},
               heidenhain::GeneralAttributeData{"CCPA", '+', _, std::string("40")},
           },
           true);

    verify("13 FC DR- R10 P1X+42.929 P1Y+60.071",
           {
               heidenhain::LineNumberData{13},
               heidenhain::GeneralAttributeData{"FC"},
               heidenhain::GeneralAttributeData{"DR", '-'},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("10")},
               heidenhain::GeneralAttributeData{"P1X", '+', _, std::string("42"), '.', std::string("929")},
               heidenhain::GeneralAttributeData{"P1Y", '+', _, std::string("60"), '.', std::string("071")},
           },
           true);

    verify("14 FLT AN-70 PDX+50 PDY+53 D10",
           {
               heidenhain::LineNumberData{14},
               heidenhain::GeneralAttributeData{"FLT"},
               heidenhain::GeneralAttributeData{"AN", '-', _, std::string("70")},
               heidenhain::GeneralAttributeData{"PDX", '+', _, std::string("50")},
               heidenhain::GeneralAttributeData{"PDY", '+', _, std::string("53")},
               heidenhain::GeneralAttributeData{"D", _, _, std::string("10")},
           },
           true);

    verify("13 FC DR- R15 CLSD+ CCX+20 CCY+35",
           {
               heidenhain::LineNumberData{13},
               heidenhain::GeneralAttributeData{"FC"},
               heidenhain::GeneralAttributeData{"DR", '-'},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("15")},
               heidenhain::GeneralAttributeData{"CLSD", '+'},
               heidenhain::GeneralAttributeData{"CCX", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"CCY", '+', _, std::string("35")},
           },
           true);

    verify("17 FCT DR- R+15 CLSD",
           {
               heidenhain::LineNumberData{17},
               heidenhain::GeneralAttributeData{"FCT"},
               heidenhain::GeneralAttributeData{"DR", '-'},
               heidenhain::GeneralAttributeData{"R", '+', _, std::string("15")},
               heidenhain::GeneralAttributeData{"CLSD"},
           },
           true);

    verify("15 FCT IX+20 DR- R20 CCA+90 RX 13",
           {
               heidenhain::LineNumberData{15},
               heidenhain::GeneralAttributeData{"FCT"},
               heidenhain::GeneralAttributeData{"IX", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"DR", '-'},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("20")},
               heidenhain::GeneralAttributeData{"CCA", '+', _, std::string("90")},
               heidenhain::GeneralAttributeData{"RX", _, _, std::string("13")},
           },
           true);

    verify("16 FL IPR+35 PA+0 RPR 13",
           {
               heidenhain::LineNumberData{16},
               heidenhain::GeneralAttributeData{"FL"},
               heidenhain::GeneralAttributeData{"IPR", '+', _, std::string("35")},
               heidenhain::GeneralAttributeData{"PA", '+', _, std::string("0")},
               heidenhain::GeneralAttributeData{"RPR", _, _, std::string("13")},
           },
           true);

    verify("19 FL LEN 12.5 PAR 17 DP 12.5",
           {
               heidenhain::LineNumberData{19},
               heidenhain::GeneralAttributeData{"FL"},
               heidenhain::GeneralAttributeData{"LEN", _, _, std::string("12"), '.', std::string("5")},
               heidenhain::GeneralAttributeData{"PAR", _, _, std::string("17")},
               heidenhain::GeneralAttributeData{"DP", _, _, std::string("12"), '.', std::string("5")},
           },
           true);

    verify("20 FSELECT 2",
           {
               heidenhain::LineNumberData{20},
               heidenhain::GeneralAttributeData{"FSELECT", _, _, std::string("2")},
           },
           true);

    verify("21 FL LEN 20 IAN+95",
           {
               heidenhain::LineNumberData{21},
               heidenhain::GeneralAttributeData{"FL"},
               heidenhain::GeneralAttributeData{"LEN", _, _, std::string("20")},
               heidenhain::GeneralAttributeData{"IAN", '+', _, std::string("95")},
           },
           true);

    verify("22 FL IAN+220 RAN 18",
           {
               heidenhain::LineNumberData{22},
               heidenhain::GeneralAttributeData{"FL"},
               heidenhain::GeneralAttributeData{"IAN", '+', _, std::string("220")},
               heidenhain::GeneralAttributeData{"RAN", _, _, std::string("18")},
           },
           true);

    verify("17 FC DR- R10 CCA+0 ICCX+20 ICCY-15 RCCX12 RCCY14",
           {
               heidenhain::LineNumberData{17},
               heidenhain::GeneralAttributeData{"FC"},
               heidenhain::GeneralAttributeData{"DR", '-'},
               heidenhain::GeneralAttributeData{"R", _, _, std::string("10")},
               heidenhain::GeneralAttributeData{"CCA", '+', _, std::string("0")},
               heidenhain::GeneralAttributeData{"ICCX", '+', _, std::string("20")},
               heidenhain::GeneralAttributeData{"ICCY", '-', _, std::string("15")},
               heidenhain::GeneralAttributeData{"RCCX", _, _, std::string("12")},
               heidenhain::GeneralAttributeData{"RCCY", _, _, std::string("14")},
           },
           true);

    verify("5 PATTERN DEF",
           {
               heidenhain::LineNumberData{5},
               heidenhain::GeneralAttributeData{"PATTERN"},
               heidenhain::GeneralAttributeData{"DEF"},
           },
           true);

    verify(R"(8 SEL PATTERN "TAB1")",
           {
               heidenhain::LineNumberData{8},
               heidenhain::GeneralAttributeData{"SEL"},
               heidenhain::GeneralAttributeData{"PATTERN"},
               heidenhain::GeneralAttributeData{R"("TAB1")"},
           },
           true);

    verify("47 L  A+Q120  C+Q122 FMAX",
           {
               heidenhain::LineNumberData{47},
               heidenhain::GeneralAttributeData{"L"},
               heidenhain::GeneralAttributeData{"A", '+', 'Q', std::string("120")},
               heidenhain::GeneralAttributeData{"C", '+', 'Q', std::string("122")},
               heidenhain::GeneralAttributeData{"FMAX"},
           },
           true);

    verify("5  PLANE SPATIAL SPA-90 SPB+Q27 SPC+90 TURN F99999 SEQ-",
           {
               heidenhain::LineNumberData{5},
               heidenhain::GeneralAttributeData{"PLANE"},
               heidenhain::GeneralAttributeData{"SPATIAL"},
               heidenhain::GeneralAttributeData{"SPA", '-', _, std::string("90")},
               heidenhain::GeneralAttributeData{"SPB", '+', 'Q', std::string("27")},
               heidenhain::GeneralAttributeData{"SPC", '+', _, std::string("90")},
               heidenhain::GeneralAttributeData{"TURN"},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("99999")},
               heidenhain::GeneralAttributeData{"SEQ", '-'},
           },
           true);

    verify("6  FN 17: SYSWRITE ID 503 NR99 IDX1 =+0",
           {
               heidenhain::LineNumberData{6},
               heidenhain::GeneralAttributeData{"FN", _, _, std::string("17")},
               heidenhain::GeneralAttributeData{":"},
               heidenhain::GeneralAttributeData{"SYSWRITE"},
               heidenhain::GeneralAttributeData{"ID", _, _, std::string("503")},
               heidenhain::GeneralAttributeData{"NR", _, _, std::string("99")},
               heidenhain::GeneralAttributeData{"IDX", _, _, std::string("1")},
               heidenhain::GeneralAttributeData{"=", '+', _, std::string("0")},
           },
           true);
}

TEST_F(AllAttributesParserHHTest, cycles)
{
    verify("6 CYCL DEF 1.0 WIERCENIE GLEB.",
           {
               heidenhain::LineNumberData{6},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("1"), '.', std::string("0")},
               heidenhain::GeneralAttributeData{"WIERCENIE"},
               heidenhain::GeneralAttributeData{"GLEB."},
           },
           true);

    verify("7 CYCL DEF 1.1 ODSTEP 2",
           {
               heidenhain::LineNumberData{7},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("1"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"ODSTEP", _, _, std::string("2")},
           },
           true);

    verify("8 CYCL DEF 1.2 GLEBOKOSC-15",
           {
               heidenhain::LineNumberData{8},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("1"), '.', std::string("2")},
               heidenhain::GeneralAttributeData{"GLEBOKOSC", '-', _, std::string("15")},
           },
           true);

    verify("10 CYCL DEF 1.4 CZAS ZWL. 1",
           {
               heidenhain::LineNumberData{10},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("1"), '.', std::string("4")},
               heidenhain::GeneralAttributeData{"CZAS"},
               heidenhain::GeneralAttributeData{"ZWL.", _, _, std::string("1")},
           },
           true);

    verify("11 CYCL DEF 1.5 F80",
           {
               heidenhain::LineNumberData{11},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("1"), '.', std::string("5")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("80")},
           },
           true);

    verify("14 CYCL DEF 4.2 GLEBOKOSC -10",
           {
               heidenhain::LineNumberData{14},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("4"), '.', std::string("2")},
               heidenhain::GeneralAttributeData{"GLEBOKOSC", '-', _, std::string("10")},
           },
           true);

    verify("15 CYCL DEF 4.3 DOSUW 4 F80",
           {
               heidenhain::LineNumberData{15},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("4"), '.', std::string("3")},
               heidenhain::GeneralAttributeData{"DOSUW", _, _, std::string("4")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("80")},
           },
           true);

    verify("16 CYCL DEF 4.4 X80",
           {
               heidenhain::LineNumberData{16},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("4"), '.', std::string("4")},
               heidenhain::GeneralAttributeData{"X", _, _, std::string("80")},
           },
           true);

    verify("18 CYCL DEF 4.6 F100 DR+ PROMIEN 10",
           {
               heidenhain::LineNumberData{18},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("4"), '.', std::string("6")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("100")},
               heidenhain::GeneralAttributeData{"DR", '+'},
               heidenhain::GeneralAttributeData{"PROMIEN", _, _, std::string("10")},

           },
           true);

    verify("5 CYCL DEF 14.1 ZNACZNIK KONTURU 1/2/3",
           {
               heidenhain::LineNumberData{5},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("14"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"ZNACZNIK"},
               heidenhain::GeneralAttributeData{"KONTURU", _, _, std::string("1")},
               heidenhain::GeneralAttributeData{"/", _, _, std::string("2")},
               heidenhain::GeneralAttributeData{"/", _, _, std::string("3")},
           },
           true);

    verify("7 CYCL DEF 30.0 ODPRACOWANIE DANYCH 3D",
           {
               heidenhain::LineNumberData{7},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("30"), '.', std::string("0")},
               heidenhain::GeneralAttributeData{"ODPRACOWANIE"},
               heidenhain::GeneralAttributeData{"DANYCH", _, _, std::string("3")}, // TODO
               heidenhain::GeneralAttributeData{"D"},                              // TODO
           },
           true);

    verify("8 CYCL DEF 30.1 PGMDIGIT.: DATNEGA",
           {
               heidenhain::LineNumberData{8},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("30"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"PGMDIGIT.:"},
               heidenhain::GeneralAttributeData{"DATNEGA"},
           },
           true);

    verify("9 CYCL DEF 30.2 X+0 Y+0 Z-35",
           {
               heidenhain::LineNumberData{9},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("30"), '.', std::string("2")},
               heidenhain::GeneralAttributeData{"X", '+', _, std::string("0")},
               heidenhain::GeneralAttributeData{"Y", '+', _, std::string("0")},
               heidenhain::GeneralAttributeData{"Z", '-', _, std::string("35")},
           },
           true);

    verify("20 CYCL DEF 8.1 Y",
           {
               heidenhain::LineNumberData{20},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("8"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"Y"},
           },
           true);

    verify("8 CYCL DEF 19.1 B+10 C+90 F1000 ABST 50",
           {
               heidenhain::LineNumberData{8},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("19"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"B", '+', _, std::string("10")},
               heidenhain::GeneralAttributeData{"C", '+', _, std::string("90")},
               heidenhain::GeneralAttributeData{"F", _, _, std::string("1000")},
               heidenhain::GeneralAttributeData{"ABST", _, _, std::string("50")},
           },
           true);

    verify("16 CYCL DEF 11.1 SCL 0.75",
           {
               heidenhain::LineNumberData{16},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("11"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"SCL", _, _, std::string("0"), '.', std::string("75")},
           },
           true);

    verify("27 CYCL DEF 26.1 X 1.4 Y 0.6 CCX+15 CCY+20",
           {
               heidenhain::LineNumberData{27},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("26"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"X", _, _, std::string("1"), '.', std::string("4")},
               heidenhain::GeneralAttributeData{"Y", _, _, std::string("0"), '.', std::string("6")},
               heidenhain::GeneralAttributeData{"CCX", '+', _, std::string("15")},
               heidenhain::GeneralAttributeData{"CCY", '+', _, std::string("20")},
           },
           true);

    verify("49 CYCL DEF 9.1 CZAS ZWL.0.5",
           {
               heidenhain::LineNumberData{49},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("9"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"CZAS"},
               heidenhain::GeneralAttributeData{"ZWL.", _, _, std::string("0"), '.', std::string("5")},
           },
           true);

    verify("7 CYCL DEF 12.0 PGM CALL",
           {
               heidenhain::LineNumberData{7},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("12"), '.', std::string("0")},
               heidenhain::GeneralAttributeData{"PGM"},
               heidenhain::GeneralAttributeData{"CALL"},
           },
           true);

    verify("8 CYCL DEF 12.1 LOT31",
           {
               heidenhain::LineNumberData{8},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("12"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"LOT", _, _, std::string("31")},
           },
           true);

    verify("9 CYCL CALL",
           {
               heidenhain::LineNumberData{9},
               heidenhain::CycleDef{"CYCL", "CALL"},
           },
           true);

    verify("9 CYCL DEF 10.1 ROT+Q150",
           {
               heidenhain::LineNumberData{9},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("10"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"ROT", '+', 'Q', std::string("150")},
           },
           true);

    verify("7  CYCL DEF 7.1 #2",
           {
               heidenhain::LineNumberData{7},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("7"), '.', std::string("1")},
               heidenhain::GeneralAttributeData{"#", _, _, std::string("2")},
           },
           true);
}

TEST_F(AllAttributesParserHHTest, cycles2)
{
    verify("11 CYCL DEF 201 ROZWIERCANIE ~",
           {
               heidenhain::LineNumberData{11},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("201")},
               heidenhain::GeneralAttributeData{"ROZWIERCANIE", _, _, _, _, _, '~'},
           },
           true);

    verify("	Q200 = 2   ;ODSTEP BEZPIECZ. ~",
           {heidenhain::CycleParam{std::string("Q"), 200, '=', _, std::string("2")},
            heidenhain::CommentData{';', "ODSTEP BEZPIECZ. ", '~'}},
           true);

    verify("	Q201 = -15 ;GLEBOKOSC",
           {heidenhain::CycleParam{std::string("Q"), 201, '=', '-', std::string("15")},
            heidenhain::CommentData{';', "GLEBOKOSC", 0}},
           true);

    verify("	Q206 = 100 ;F DOSUW WGL.",
           {heidenhain::CycleParam{std::string("Q"), 206, '=', _, std::string("100")},
            heidenhain::CommentData{';', "F DOSUW WGL.", 0}},
           true);

    verify("	Q211 = 0.5 ;CZAS ZWL. NA DOLE",
           {heidenhain::CycleParam{std::string("Q"), 211, '=', _, std::string("0"), '.', std::string("5")},
            heidenhain::CommentData{';', "CZAS ZWL. NA DOLE", 0}},
           true);

    verify("	Q203 = +0  ;WSPL.POWIERZ.",
           {heidenhain::CycleParam{std::string("Q"), 203, '=', '+', std::string("0")},
            heidenhain::CommentData{';', "WSPL.POWIERZ.", 0}},
           true);

    verify("	Q204 = 100 ;2. ODST.BEZP.",
           {heidenhain::CycleParam{std::string("Q"), 204, '=', _, std::string("100")},
            heidenhain::CommentData{';', "2. ODST.BEZP.", 0}},
           true);

    verify("8  CYCL DEF 220 POLAR PATTERN ~",
           {
               heidenhain::LineNumberData{8},
               heidenhain::CycleDef{"CYCL", "DEF", std::string("220")},
               heidenhain::GeneralAttributeData{"POLAR"},
               heidenhain::GeneralAttributeData{"PATTERN", _, _, _, _, _, '~'},
           },
           true);
}

} // namespace heidenhain_test
