#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

namespace parser {
namespace fanuc {

class MathUtils
{
public:
    static double CalcLineLength2d(double X, double Y)
    {
        return sqrt(pow(X, 2) + pow(Y, 2));
    }

    static double CalcLineLength2d(double Xpre, double Ypre, double Xcur, double Ycur)
    {
        return CalcLineLength2d(Xpre - Xcur, Ypre - Ycur);
    }

    static double CalcLineLength3d(double X, double Y, double Z)
    {
        return sqrt(pow(X, 2) + pow(Y, 2) + pow(Z, 2));
    }

    static double CalcLineLength3d(double Xpre, double Ypre, double Zpre, double Xcur, double Ycur, double Zcur)
    {
        return CalcLineLength3d(Xpre - Xcur, Ypre - Ycur, Zpre - Zcur);
    }

    static double CalcCircleLength2d(double X, double Y)
    {
        return 2 * M_PI * CalcLineLength2d(X, Y);
    }

    static double CalcTime(double Path, double Feed)
    {
        return Path / Feed;
    }

    static double CalcFeedMinutes(double F, double S)
    {
        return F * S;
    }

    static double CalcTimeCSSmm(double S, double Path)
    {
        return (S * 1000) / (M_PI * Path);
    }

    static double CalcTimeCSSinch(double S, double Path)
    {
        return (S * 12) / (M_PI * Path);
    }
};

} // namespace fanuc
} // namespace parser
