#include "stdafx.h"

#include <gtest/gtest.h>

#include <fanuc/PathCalculator.h>

using namespace parser;

namespace fanuc_test {

class PathCalculatorTest : public ::testing::Test
{
protected:
};

TEST_F(PathCalculatorTest, G0)
{
    const double tolerance = 1e-8;
    EXPECT_NEAR(13.856406460551, fanuc::PathCalculator::G0(12, 13, 14, 20, 21, 22), tolerance);
    EXPECT_NEAR(13.856406460551, fanuc::PathCalculator::G0(8, 8, 8), tolerance);
}

TEST_F(PathCalculatorTest, G1)
{
    const double tolerance = 1e-8;
    EXPECT_NEAR(13.856406460551, fanuc::PathCalculator::G1(12, 13, 14, 20, 21, 22), tolerance);
    EXPECT_NEAR(13.856406460551, fanuc::PathCalculator::G1(8, 8, 8), tolerance);
}

TEST_F(PathCalculatorTest, G2G3)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double, double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2_G17), static_cast<fn*>(&fanuc::PathCalculator::G3_G17),
        static_cast<fn*>(&fanuc::PathCalculator::G2_G18), static_cast<fn*>(&fanuc::PathCalculator::G3_G18),
        static_cast<fn*>(&fanuc::PathCalculator::G2_G19), static_cast<fn*>(&fanuc::PathCalculator::G3_G19),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(2.82954938327595, f(12., 13., 14., 15., 20., 21.), tolerance);
}

TEST_F(PathCalculatorTest, G2_Increment)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2_G17),
        static_cast<fn*>(&fanuc::PathCalculator::G2_G18),
        static_cast<fn*>(&fanuc::PathCalculator::G2_G19),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(20.9723231255513, f(14., 15., 20., 21.), tolerance);
}

TEST_F(PathCalculatorTest, G3_Increment)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G3_G17),
        static_cast<fn*>(&fanuc::PathCalculator::G3_G18),
        static_cast<fn*>(&fanuc::PathCalculator::G3_G19),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(161.240050782657, f(14., 15., 20., 21.), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_R)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_G17),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_G18),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_G19),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(2.83078946648854, f(12., 13., 14., 15., 20.), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_R_Increment)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_G17),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_G18),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_G19),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(21.5450445774531, f(14., 15., 20.), tolerance);
}

TEST_F(PathCalculatorTest, Drill_CyclePath_DepthIncremental)
{
    const double tolerance = 1e-8;

    using fn = std::pair<double, double>(double, double, double, EDepthProgrammingType, EDrillGmode, EDrillGreturnMode);
    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G17),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G18),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G19),
    };

    for (const auto drill_gmode :
         {EDrillGmode::G81, EDrillGmode::G82, EDrillGmode::G85, EDrillGmode::G86, EDrillGmode::G88, EDrillGmode::G89})
    {
        for (const auto& drill_greturn_mode : {std::make_pair(EDrillGreturnMode::G98, std::make_pair(27., 13.)),
                                               std::make_pair(EDrillGreturnMode::G99, std::make_pair(20., 13.))})
        {
            for (auto& f : func_vec)
            {
                const auto ret =
                    f(14., 20., 21., EDepthProgrammingType::Incremental, drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }

    for (const auto drill_gmode : {EDrillGmode::G74, EDrillGmode::G84})
    {
        for (const auto& drill_greturn_mode : {std::make_pair(EDrillGreturnMode::G98, std::make_pair(40., 14.)),
                                               std::make_pair(EDrillGreturnMode::G99, std::make_pair(40., 7.))})
        {
            for (auto& f : func_vec)
            {
                const auto ret =
                    f(14., 20., 21., EDepthProgrammingType::Incremental, drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }
}

TEST_F(PathCalculatorTest, Drill_CyclePath_Absolute)
{
    const double tolerance = 1e-8;

    using fn = std::pair<double, double>(double, double, double, EDepthProgrammingType, EDrillGmode, EDrillGreturnMode);
    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G17),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G18),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G19),
    };

    for (const auto drill_gmode :
         {EDrillGmode::G81, EDrillGmode::G82, EDrillGmode::G85, EDrillGmode::G86, EDrillGmode::G88, EDrillGmode::G89})
    {
        for (const auto& drill_greturn_mode : {std::make_pair(EDrillGreturnMode::G98, std::make_pair(1., 13.)),
                                               std::make_pair(EDrillGreturnMode::G99, std::make_pair(1., 6.))})
        {
            for (auto& f : func_vec)
            {
                const auto ret =
                    f(14., 20., 21., EDepthProgrammingType::Absolute, drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }

    for (const auto drill_gmode : {EDrillGmode::G74, EDrillGmode::G84})
    {
        for (const auto& drill_greturn_mode : {std::make_pair(EDrillGreturnMode::G98, std::make_pair(2., 14.)),
                                               std::make_pair(EDrillGreturnMode::G99, std::make_pair(2., 7.))})
        {
            for (auto& f : func_vec)
            {
                const auto ret =
                    f(14., 20., 21., EDepthProgrammingType::Absolute, drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }
}

TEST_F(PathCalculatorTest, Drill_CyclePath_Incremental)
{
    const double tolerance = 1e-8;

    using fn = std::pair<double, double>(double, double, EDrillGmode, EDrillGreturnMode);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G17),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G18),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G19),
    };

    for (const auto drill_gmode :
         {EDrillGmode::G81, EDrillGmode::G82, EDrillGmode::G85, EDrillGmode::G86, EDrillGmode::G88, EDrillGmode::G89})
    {
        for (const auto& drill_greturn_mode : {std::make_pair(EDrillGreturnMode::G98, std::make_pair(6., 48.)),
                                               std::make_pair(EDrillGreturnMode::G99, std::make_pair(6., 27.))})
        {
            for (auto& f : func_vec)
            {
                const auto  ret          = f(6., 21., drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }

    for (const auto drill_gmode : {EDrillGmode::G74, EDrillGmode::G84})
    {
        for (const auto& drill_greturn_mode : {std::make_pair(EDrillGreturnMode::G98, std::make_pair(12., 42.)),
                                               std::make_pair(EDrillGreturnMode::G99, std::make_pair(12., 0.))})
        {
            for (auto& f : func_vec)
            {
                const auto  ret          = f(6., 21., drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }
}

TEST_F(PathCalculatorTest, Drill_CyclePath_QParam_DepthIncremental)
{
    const double tolerance = 1e-8;

    using fn = std::pair<double, double>(double, double, double, double, EDepthProgrammingType, EDrillGmode,
                                         EDrillGreturnMode);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G17),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G18),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G19),
    };

    for (const auto& data : {
             std::make_tuple(EDrillGmode::G87, std::make_pair(EDrillGreturnMode::G98, std::make_pair(1., 33.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(1., 33.))),
             std::make_tuple(EDrillGmode::G76, std::make_pair(EDrillGreturnMode::G98, std::make_pair(20., 44.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(20., 37.))),
             std::make_tuple(EDrillGmode::G73, std::make_pair(EDrillGreturnMode::G98, std::make_pair(24., 34.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(24., 27.))),
             std::make_tuple(EDrillGmode::G83, std::make_pair(EDrillGreturnMode::G98, std::make_pair(24., 90.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(24., 83.))),
         })
    {
        auto& [drill_gmode, drill_greturn_mode98, drill_greturn_mode99] = data;
        for (const auto& drill_greturn_mode : {drill_greturn_mode98, drill_greturn_mode99})
        {
            for (auto& f : func_vec)
            {
                const auto ret =
                    f(14., 20., 21., 5., EDepthProgrammingType::Incremental, drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }
}

TEST_F(PathCalculatorTest, Drill_CyclePath_QParam_Absolute)
{
    const double tolerance = 1e-8;

    using fn = std::pair<double, double>(double, double, double, double, EDepthProgrammingType, EDrillGmode,
                                         EDrillGreturnMode);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G17),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G18),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G19),
    };

    for (const auto& data : {
             std::make_tuple(EDrillGmode::G87, std::make_pair(EDrillGreturnMode::G98, std::make_pair(1., 33.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(1., 33.))),
             std::make_tuple(EDrillGmode::G76, std::make_pair(EDrillGreturnMode::G98, std::make_pair(1., 23.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(1., 18.))),
             std::make_tuple(EDrillGmode::G73, std::make_pair(EDrillGreturnMode::G98, std::make_pair(1., 15.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(1., 8.))),
             std::make_tuple(EDrillGmode::G83, std::make_pair(EDrillGreturnMode::G98, std::make_pair(1., 15.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(1., 8.))),
         })
    {
        auto& [drill_gmode, drill_greturn_mode98, drill_greturn_mode99] = data;
        for (const auto& drill_greturn_mode : {drill_greturn_mode98, drill_greturn_mode99})
        {
            for (auto& f : func_vec)
            {
                const auto ret =
                    f(14., 20., 21., 5., EDepthProgrammingType::Absolute, drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }
}

TEST_F(PathCalculatorTest, Drill_CyclePath_QParam_Incremental)
{
    const double tolerance = 1e-8;

    using fn = std::pair<double, double>(double, double, double, EDrillGmode, EDrillGreturnMode);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G17),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G18),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePath_G19),
    };

    for (const auto& data : {
             std::make_tuple(EDrillGmode::G87,
                             std::make_pair(EDrillGreturnMode::G98, std::make_pair(6., 64.8102496759066)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(6., 64.8102496759066))),
             std::make_tuple(EDrillGmode::G76, std::make_pair(EDrillGreturnMode::G98, std::make_pair(6., 37.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(6., 16.))),
             std::make_tuple(EDrillGmode::G73, std::make_pair(EDrillGreturnMode::G98, std::make_pair(7., 49.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(7., 28.))),
             std::make_tuple(EDrillGmode::G83, std::make_pair(EDrillGreturnMode::G98, std::make_pair(7., 57.)),
                             std::make_pair(EDrillGreturnMode::G99, std::make_pair(7., 36.))),
         })
    {
        auto& [drill_gmode, drill_greturn_mode98, drill_greturn_mode99] = data;
        for (const auto& drill_greturn_mode : {drill_greturn_mode98, drill_greturn_mode99})
        {
            for (auto& f : func_vec)
            {
                const auto  ret          = f(6., 21., 5., drill_gmode, drill_greturn_mode.first);
                const auto& ret_expected = drill_greturn_mode.second;
                EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
                EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
            }
        }
    }
}

TEST_F(PathCalculatorTest, Drill_CyclePathHole)
{
    const double tolerance = 1e-8;

    using fn = std::pair<double, double>(double, double, double, double, const std::pair<double, double>&);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePathHole_G17),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePathHole_G18),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePathHole_G19),
    };

    const auto cycle_distance = std::make_pair(10., 15.);
    const auto ret_expected   = std::make_pair(10., 40.45584412271571);
    for (auto& f : func_vec)
    {
        const auto ret = f(12., 13., 30., 31., cycle_distance);
        EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
        EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
    }
}

TEST_F(PathCalculatorTest, Drill_CyclePathHole_Incremental)
{
    const double tolerance = 1e-8;

    using fn = std::pair<double, double>(double, double, const std::pair<double, double>&);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePathHole_G17),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePathHole_G18),
        static_cast<fn*>(&fanuc::PathCalculator::DrillCyclePathHole_G19),
    };

    const auto cycle_distance = std::make_pair(10., 15.);
    const auto ret_expected   = std::make_pair(10., 40.45584412271571);
    for (auto& f : func_vec)
    {
        const auto ret = f(18., 18., cycle_distance);
        EXPECT_NEAR(ret_expected.first, ret.first, tolerance);
        EXPECT_NEAR(ret_expected.second, ret.second, tolerance);
    }
}

TEST_F(PathCalculatorTest, G2G3_Helix)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(271.125088435787, f(12., 13., 30., 31.), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_Helix_Incremental)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(271.125088435787, f(-1., 30., 31.), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_Helix_R)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17R),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18R),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19R),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(188.600249897836, f(12., 13., 30.), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_Helix_R_Incremental)
{
    const double tolerance = 1e-8;

    using fn = double(double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17R),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18R),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19R),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(188.600249897836, f(-1., 30.), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_HelixQuarter)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17_Quarter),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18_Quarter),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19_Quarter),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(68.0537424776271, f(12., 13., 30., 31.), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_HelixQuarter_Incremental)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17_Quarter),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18_Quarter),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19_Quarter),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(68.0537424776271, f(-1., 30., 31.), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_HelixQuarter_R)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17_QuarterR),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18_QuarterR),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19_QuarterR),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(47.5409235064851, f(12., 13., 30), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_HelixQuarter_R_Incremental)
{
    const double tolerance = 1e-8;

    using fn = double(double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17_QuarterR),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18_QuarterR),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19_QuarterR),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(47.5409235064851, f(-1., 30), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_HelixLast)
{
    const double tolerance = 1e-8;
    EXPECT_NEAR(6.173, fanuc::PathCalculator::G2G3_HelixLast(12.346), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_Helix_Archimedes_Q)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17_ArchimedesQparam),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18_ArchimedesQparam),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19_ArchimedesQparam),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(38.773935717471, f(12., 13., 30), tolerance);
}

TEST_F(PathCalculatorTest, G2G3_Helix_Archimedes_L)
{
    const double tolerance = 1e-8;

    using fn = double(double, double, double);

    std::vector<std::function<fn>> func_vec = {
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG17_ArchimedesLparam),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG18_ArchimedesLparam),
        static_cast<fn*>(&fanuc::PathCalculator::G2G3_HelixG19_ArchimedesLparam),
    };

    for (auto& f : func_vec)
        EXPECT_NEAR(1667.71529169112, f(12., 13., 30), tolerance);
}

} // namespace fanuc_test
