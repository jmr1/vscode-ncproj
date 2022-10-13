#pragma once

#include "parser_export.h"

#include <cmath>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "GeneralParserDefines.h"
#include "MathUtils.h"

namespace parser {
namespace fanuc {

class PARSER_API PathCalculator
{
public:
    // Ruch szybki G0
    static double G0(double X, double Y, double Z)
    {
        return MathUtils::CalcLineLength3d(X, Y, Z);
    }

    static double G0(double Xpre, double Ypre, double Zpre, double Xcur, double Ycur, double Zcur)
    {
        return MathUtils::CalcLineLength3d(Xpre, Ypre, Zpre, Xcur, Ycur, Zcur);
    }

    // Ruch liniowy G1
    static double G1(double X, double Y, double Z)
    {
        return MathUtils::CalcLineLength3d(X, Y, Z);
    }

    static double G1(double Xpre, double Ypre, double Zpre, double Xcur, double Ycur, double Zcur)
    {
        return MathUtils::CalcLineLength3d(Xpre, Ypre, Zpre, Xcur, Ycur, Zcur);
    }

    // Ruch kolowy G2 lub G3 - G17 IJ
    static double G2_G17(double Xpre, double Ypre, double Xcur, double Ycur, double Icur, double Jcur)
    {
        return G17(Xpre, Ypre, Xcur, Ycur, Icur, Jcur, 2);
    }

    static double G2_G17(double Xcur, double Ycur, double Icur, double Jcur)
    {
        return G17(Xcur, Ycur, Icur, Jcur, 2);
    }

    static double G3_G17(double Xpre, double Ypre, double Xcur, double Ycur, double Icur, double Jcur)
    {
        return G17(Xpre, Ypre, Xcur, Ycur, Icur, Jcur, 3);
    }

    static double G3_G17(double Xcur, double Ycur, double Icur, double Jcur)
    {
        return G17(Xcur, Ycur, Icur, Jcur, 3);
    }

    // Ruch kolowy G2 lub G3 - G18 IK
    static double G2_G18(double Xpre, double Zpre, double Xcur, double Zcur, double Icur, double Kcur)
    {
        return G18(Xpre, Zpre, Xcur, Zcur, Icur, Kcur, 2);
    }

    static double G2_G18(double Xcur, double Zcur, double Icur, double Kcur)
    {
        return G18(Xcur, Zcur, Icur, Kcur, 2);
    }

    static double G3_G18(double Xpre, double Zpre, double Xcur, double Zcur, double Icur, double Kcur)
    {
        return G18(Xpre, Zpre, Xcur, Zcur, Icur, Kcur, 3);
    }

    static double G3_G18(double Xcur, double Zcur, double Icur, double Kcur)
    {
        return G18(Xcur, Zcur, Icur, Kcur, 3);
    }

    // Ruch kolowy G2 lub G3 - G19 JK
    static double G2_G19(double Ypre, double Zpre, double Ycur, double Zcur, double Jcur, double Kcur)
    {
        return G19(Ypre, Zpre, Ycur, Zcur, Jcur, Kcur, 2);
    }

    static double G2_G19(double Ycur, double Zcur, double Jcur, double Kcur)
    {
        return G19(Ycur, Zcur, Jcur, Kcur, 2);
    }

    static double G3_G19(double Ypre, double Zpre, double Ycur, double Zcur, double Jcur, double Kcur)
    {
        return G19(Ypre, Zpre, Ycur, Zcur, Jcur, Kcur, 3);
    }

    static double G3_G19(double Ycur, double Zcur, double Jcur, double Kcur)
    {
        return G19(Ycur, Zcur, Jcur, Kcur, 3);
    }

    // Ruch kolowy G2 lub G3 - G17 - R
    static double G2G3_G17(double Xpre, double Ypre, double Xcur, double Ycur, double Rcur)
    {
        return G17(Xpre, Ypre, Xcur, Ycur, Rcur);
    }

    static double G2G3_G17(double Xcur, double Ycur, double Rcur)
    {
        return G17(Xcur, Ycur, Rcur);
    }

    // Ruch kolowy G2 lub G3 - G18 - R
    static double G2G3_G18(double Xpre, double Zpre, double Xcur, double Zcur, double Rcur)
    {
        return G18(Xpre, Zpre, Xcur, Zcur, Rcur);
    }

    static double G2G3_G18(double Xcur, double Zcur, double Rcur)
    {
        return G18(Xcur, Zcur, Rcur);
    }

    // Ruch kolowy G2 lub G3 - G19 - R
    static double G2G3_G19(double Ypre, double Zpre, double Ycur, double Zcur, double Rcur)
    {
        return G19(Ypre, Zpre, Ycur, Zcur, Rcur);
    }

    static double G2G3_G19(double Ycur, double Zcur, double Rcur)
    {
        return G19(Ycur, Zcur, Rcur);
    }

    // Spirala
    static double G2G3_HelixG17(double Zpre, double Zcur, double Icur, double Jcur);
    static double G2G3_HelixG17(double Zcur, double Icur, double Jcur);
    static double G2G3_HelixG17_Quarter(double Zpre, double Zcur, double Icur, double Jcur);
    static double G2G3_HelixG17_Quarter(double Zcur, double Icur, double Jcur);
    static double G2G3_HelixG17R(double Zpre, double Zcur, double R);
    static double G2G3_HelixG17R(double Zcur, double R);
    static double G2G3_HelixG17_QuarterR(double Zpre, double Zcur, double R);
    static double G2G3_HelixG17_QuarterR(double Zcur, double R);
    static double G2G3_HelixLast(double HLength);

    static double G2G3_HelixG18(double Ypre, double Ycur, double Icur, double Kcur)
    {
        return G2G3_HelixG17(Ypre, Ycur, Icur, Kcur);
    }

    static double G2G3_HelixG18(double Ycur, double Icur, double Kcur)
    {
        return G2G3_HelixG17(Ycur, Icur, Kcur);
    }

    static double G2G3_HelixG18_Quarter(double Ypre, double Ycur, double Icur, double Kcur)
    {
        return G2G3_HelixG17_Quarter(Ypre, Ycur, Icur, Kcur);
    }

    static double G2G3_HelixG18_Quarter(double Ycur, double Icur, double Kcur)
    {
        return G2G3_HelixG17_Quarter(Ycur, Icur, Kcur);
    }

    static double G2G3_HelixG18R(double Ypre, double Ycur, double R)
    {
        return G2G3_HelixG17R(Ypre, Ycur, R);
    }

    static double G2G3_HelixG18R(double Ycur, double R)
    {
        return G2G3_HelixG17R(Ycur, R);
    }

    static double G2G3_HelixG18_QuarterR(double Ypre, double Ycur, double R)
    {
        return G2G3_HelixG17_QuarterR(Ypre, Ycur, R);
    }

    static double G2G3_HelixG18_QuarterR(double Ycur, double R)
    {
        return G2G3_HelixG17_QuarterR(Ycur, R);
    }

    static double G2G3_HelixG19(double Xpre, double Xcur, double Jcur, double Kcur)
    {
        return G2G3_HelixG17(Xpre, Xcur, Jcur, Kcur);
    }

    static double G2G3_HelixG19(double Xcur, double Jcur, double Kcur)
    {
        return G2G3_HelixG17(Xcur, Jcur, Kcur);
    }

    static double G2G3_HelixG19_Quarter(double Xpre, double Xcur, double Jcur, double Kcur)
    {
        return G2G3_HelixG17_Quarter(Xpre, Xcur, Jcur, Kcur);
    }

    static double G2G3_HelixG19_Quarter(double Xcur, double Jcur, double Kcur)
    {
        return G2G3_HelixG17_Quarter(Xcur, Jcur, Kcur);
    }

    static double G2G3_HelixG19R(double Xpre, double Xcur, double R)
    {
        return G2G3_HelixG17R(Xpre, Xcur, R);
    }

    static double G2G3_HelixG19R(double Xcur, double R)
    {
        return G2G3_HelixG17R(Xcur, R);
    }

    static double G2G3_HelixG19_QuarterR(double Xpre, double Xcur, double R)
    {
        return G2G3_HelixG17_QuarterR(Xpre, Xcur, R);
    }

    static double G2G3_HelixG19_QuarterR(double Xcur, double R)
    {
        return G2G3_HelixG17_QuarterR(Xcur, R);
    }

    // L number of revolutions (Liczba zwojow)
    static double G2G3_HelixG17_ArchimedesLparam(double Icur, double Jcur, double Lparam);
    // Q radius increment or decrement per revolution (Przyrost Q)
    static double G2G3_HelixG17_ArchimedesQparam(double Icur, double Jcur, double Qparam);

    static double G2G3_HelixG18_ArchimedesLparam(double Icur, double Kcur, double Lparam)
    {
        return G2G3_HelixG17_ArchimedesLparam(Icur, Kcur, Lparam);
    }

    static double G2G3_HelixG18_ArchimedesQparam(double Icur, double Kcur, double Qparam)
    {
        return G2G3_HelixG17_ArchimedesQparam(Icur, Kcur, Qparam);
    }

    static double G2G3_HelixG19_ArchimedesLparam(double Jcur, double Kcur, double Lparam)
    {
        return G2G3_HelixG17_ArchimedesLparam(Jcur, Kcur, Lparam);
    }

    static double G2G3_HelixG19_ArchimedesQparam(double Jcur, double Kcur, double Qparam)
    {
        return G2G3_HelixG17_ArchimedesQparam(Jcur, Kcur, Qparam);
    }

    // Uwaga jesli w kodzie nie bedzie podana funkcja G98 lub G99 zastosowac domyslny tryb G98
    // Uwaga! Adres Z w cyklu wiertarskim ma inne znaczenie niz Z w ruchu G0, G1 czy G2/G3
    // Rparam - parametr R odjazd z dna otworu na wysokosc bezpieczna - odskok
    // Pparam - parametr przerwy czasowej
    // Qparam - wielkosc odjazdu przy wierceniu glebokim - wycofanie aby usunac wiora
    // pair<work motion, fast motion>
    static std::pair<double, double> DrillCyclePath_G17(double Zpre, double Zgleb, double Rparam,
                                                        EDepthProgrammingType depthProgrammingType, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98);
    static std::pair<double, double> DrillCyclePath_G17(double Zpre, double Zgleb, double Rparam, double Qparam,
                                                        EDepthProgrammingType depthProgrammingType, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98);
    static std::pair<double, double> DrillCyclePath_G17(double Zgleb, double Rparam, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98);
    static std::pair<double, double> DrillCyclePath_G17(double Zgleb, double Rparam, double Qparam, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98);
    // Pierwszy otwór jest liczony w DrillCyclePath, stąd zaczynamy od hole_number = 2
    static std::pair<double, double> DrillCyclePathHole_G17(double Xpre, double Ypre, double Xcur, double Ycur,
                                                            const std::pair<double, double>& Cycle_distance);
    static std::pair<double, double> DrillCyclePathHole_G17(double X, double Y,
                                                            const std::pair<double, double>& Cycle_distance);

    static std::pair<double, double> DrillCyclePath_G18(double Ypre, double Ygleb, double Rparam,
                                                        EDepthProgrammingType depthProgrammingType, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98)
    {
        return DrillCyclePath_G17(Ypre, Ygleb, Rparam, depthProgrammingType, Gmode, GreturnMode);
    }

    static std::pair<double, double> DrillCyclePath_G18(double Ypre, double Ygleb, double Rparam, double Qparam,
                                                        EDepthProgrammingType depthProgrammingType, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98)
    {
        return DrillCyclePath_G17(Ypre, Ygleb, Rparam, Qparam, depthProgrammingType, Gmode, GreturnMode);
    }

    static std::pair<double, double> DrillCyclePath_G18(double Ygleb, double Rparam, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98)
    {
        return DrillCyclePath_G17(Ygleb, Rparam, Gmode, GreturnMode);
    }

    static std::pair<double, double> DrillCyclePath_G18(double Ygleb, double Rparam, double Qparam, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98)
    {
        return DrillCyclePath_G17(Ygleb, Rparam, Qparam, Gmode, GreturnMode);
    }

    static std::pair<double, double> DrillCyclePathHole_G18(double Xpre, double Zpre, double Xcur, double Zcur,
                                                            const std::pair<double, double>& Cycle_distance)
    {
        return DrillCyclePathHole_G17(Xpre, Zpre, Xcur, Zcur, Cycle_distance);
    }

    static std::pair<double, double> DrillCyclePathHole_G18(double X, double Z,
                                                            const std::pair<double, double>& Cycle_distance)
    {
        return DrillCyclePathHole_G17(X, Z, Cycle_distance);
    }

    static std::pair<double, double> DrillCyclePath_G19(double Xpre, double Xgleb, double Rparam,
                                                        EDepthProgrammingType depthProgrammingType, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98)
    {
        return DrillCyclePath_G17(Xpre, Xgleb, Rparam, depthProgrammingType, Gmode, GreturnMode);
    }

    static std::pair<double, double> DrillCyclePath_G19(double Xpre, double Xgleb, double Rparam, double Qparam,
                                                        EDepthProgrammingType depthProgrammingType, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98)
    {
        return DrillCyclePath_G17(Xpre, Xgleb, Rparam, Qparam, depthProgrammingType, Gmode, GreturnMode);
    }

    static std::pair<double, double> DrillCyclePath_G19(double Xgleb, double Rparam, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98)
    {
        return DrillCyclePath_G17(Xgleb, Rparam, Gmode, GreturnMode);
    }

    static std::pair<double, double> DrillCyclePath_G19(double Xgleb, double Rparam, double Qparam, EDrillGmode Gmode,
                                                        EDrillGreturnMode GreturnMode = EDrillGreturnMode::G98)
    {
        return DrillCyclePath_G17(Xgleb, Rparam, Qparam, Gmode, GreturnMode);
    }

    static std::pair<double, double> DrillCyclePathHole_G19(double Ypre, double Zpre, double Ycur, double Zcur,
                                                            const std::pair<double, double>& Cycle_distance)
    {
        return DrillCyclePathHole_G17(Ypre, Zpre, Ycur, Zcur, Cycle_distance);
    }

    static std::pair<double, double> DrillCyclePathHole_G19(double X, double Z,
                                                            const std::pair<double, double>& Cycle_distance)
    {
        return DrillCyclePathHole_G17(X, Z, Cycle_distance);
    }

private:
    static double G17(double Xpre, double Ypre, double Xcur, double Ycur, double Icur, double Jcur,
                      int Direction_CW_CCW);
    static double G17(double Xcur, double Ycur, double Icur, double Jcur, int Direction_CW_CCW);

    static double G18(double Xpre, double Zpre, double Xcur, double Zcur, double Icur, double Kcur,
                      int Direction_CW_CCW)
    {
        return G17(Xpre, Zpre, Xcur, Zcur, Icur, Kcur, Direction_CW_CCW);
    }

    static double G18(double Xcur, double Zcur, double Icur, double Kcur, int Direction_CW_CCW)
    {
        return G17(Xcur, Zcur, Icur, Kcur, Direction_CW_CCW);
    }

    static double G19(double Ypre, double Zpre, double Ycur, double Zcur, double Jcur, double Kcur,
                      int Direction_CW_CCW)
    {
        return G17(Ypre, Zpre, Ycur, Zcur, Jcur, Kcur, Direction_CW_CCW);
    }

    static double G19(double Ycur, double Zcur, double Jcur, double Kcur, int Direction_CW_CCW)
    {
        return G17(Ycur, Zcur, Jcur, Kcur, Direction_CW_CCW);
    }

    static double G17(double Xpre, double Ypre, double Xcur, double Ycur, double Rcur);
    static double G17(double Xcur, double Ycur, double Rcur);

    static double G18(double Xpre, double Zpre, double Xcur, double Zcur, double Rcur)
    {
        return G17(Xpre, Zpre, Xcur, Zcur, Rcur);
    }

    static double G18(double Xcur, double Zcur, double Rcur)
    {
        return G17(Xcur, Zcur, Rcur);
    }

    static double G19(double Ypre, double Zpre, double Ycur, double Zcur, double Rcur)
    {
        return G17(Ypre, Zpre, Ycur, Zcur, Rcur);
    }

    static double G19(double Ycur, double Zcur, double Rcur)
    {
        return G17(Ycur, Zcur, Rcur);
    }

    static double HelixG17(double HPitch, double Icur, double Jcur);
    static double HelixG17_Quarter(double HPitch, double Icur, double Jcur);
    static double HelixG17R(double HPitch, double R);
    static double HelixG17_QuarterR(double HPitch, double R);

    static double G2G3_Helix_Archimedes(double Lparam, double HRadius);
};

} // namespace fanuc
} // namespace parser
