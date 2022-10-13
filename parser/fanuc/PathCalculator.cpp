#include "stdafx.h"

#include <cmath>

#include "PathCalculator.h"

namespace parser {
namespace fanuc {

constexpr double Deg = 180 / M_PI;

double PathCalculator::G17(double Xpre, double Ypre, double Xcur, double Ycur, double Icur, double Jcur,
                           int Direction_CW_CCW)
{
    //-- Obliczenia dlugosci luku w plaszczyznie G17 XYIJ --

    // 1. Obliczenie cieciwy luku AB
    auto AB = MathUtils::CalcLineLength2d(Xpre, Ypre, Xcur, Ycur);

    // 2. Obliczenie polowy cieciwy
    auto ABhalf = AB / 2;

    // 3. Obliczenie punktu srodka luku
    auto Xcenter = Xpre + Icur;
    auto Ycenter = Ypre + Jcur;

    // 4. Obliczenie promienia R luku
    auto Rarcradius = sqrt(pow((Xcenter - Xpre), 2) + pow((Ycenter - Ypre), 2));

    // 5. Obliczenie polowy okregu dla danego luku
    auto Semicircle = (2 * M_PI * Rarcradius) / 2;

    // Warunek: jesli Semicircle = Rarcradius to luk ma 180stopni i dlugosc jego wynosi promien
    double tolerance = 1e-8;
    double Arclong{};
    if (abs(Semicircle - Rarcradius) < tolerance)
    {
        Arclong = Rarcradius;
    }
    else
    {
        // 6. Obliczenie wysokosci H
        auto Htriangle = sqrt(std::abs(pow(Rarcradius, 2) - pow(ABhalf, 2)));

        // 7. Obliczenie kata cieciwy AB i promienia R - UWAGA  ATAN2(a,b) = ATAN(b/a) z wyjatkiem gdy a=0
        auto AangleHR = atan(ABhalf / Htriangle) * Deg;

        // 8. Obliczenie kata calego luku
        auto AlphaArc = AangleHR * 2;

        //------- Sprawdzenie czy luk jest wiekszy niz 180 stopni. Jesli tak to longArc=1
        //  Przeniesienie wspolrzednych do poczatku ukladu 0,0

        double PretempX{}, PretempY{}, CurtempX{}, CurtempY{};
        if (Xcenter != 0 || Ycenter != 0)
        {
            PretempX = Xpre - Xcenter;
            PretempY = Ypre - Ycenter;
            CurtempX = Xcur - Xcenter;
            CurtempY = Ycur - Ycenter;
        }
        else
        {
            PretempX = Xpre;
            PretempY = Ypre;
            CurtempX = Xcur;
            CurtempY = Ycur;
        }

        // Sprawdzenie do której cwiartki nalezy punkt poprzedni
        // Utworzenie grupy cwiartek poprzednich Prev
        int Prev{};
        if (PretempX > 0 && PretempY > 0)
            Prev = 1;
        else if (PretempX < 0 && PretempY < 0)
            Prev = 3;
        else if (PretempX > 0 && PretempY < 0)
            Prev = 4;
        else if (PretempX < 0 && PretempY > 0)
            Prev = 2;

        // Sprawdzenie do ktorej cwiartki nalezy punkt biezacy
        // Utworzenie grup cwiartek biezacych Curr
        int Curr{};
        if (CurtempX > 0 && CurtempY > 0)
            Curr = 1;
        else if (CurtempX < 0 && CurtempY < 0)
            Curr = 3;
        else if (CurtempX > 0 && CurtempY < 0)
            Curr = 4;
        else if (CurtempX < 0 && CurtempY > 0)
            Curr = 2;

        // Sprawdzenie czy luk przechodzi przez 4 cwiartki
        bool longArc{};
        if (Direction_CW_CCW == 3)
        {
            if ((Prev == 1 && Curr == 4) || (Prev == 2 && Curr == 1) || (Prev == 3 && Curr == 2) ||
                (Prev == 4 && Curr == 3))
            {
                longArc = true;
            }
        }
        else if (Direction_CW_CCW == 2)
        {
            if ((Prev == 4 && Curr == 1) || (Prev == 1 && Curr == 2) || (Prev == 2 && Curr == 3) ||
                (Prev == 3 && Curr == 4))
            {
                longArc = true;
            }
        }

        // Sprawdzanie czy luk przechodzac przez 3 cwiartki jest wiekszy niz 180 stopni
        // Najpierw obliczenie tg dla punktow poprzednich i biezacych
        auto tg_p = atan(PretempY / PretempX) * Deg;
        auto tg_c = atan(CurtempY / CurtempX) * Deg;

        if (Direction_CW_CCW == 3)
        {
            if ((Prev == 1 && Curr == 3 && tg_c > tg_p) || (Prev == 3 && Curr == 1 && tg_c > tg_p) ||
                (Prev == 2 && Curr == 4 && tg_c > tg_p) || (Prev == 4 && Curr == 2 && tg_c > tg_p))
            {
                longArc = true;
            }
        }
        else if (Direction_CW_CCW == 2)
        {
            if ((Prev == 1 && Curr == 3 && tg_c < tg_p) || (Prev == 3 && Curr == 1 && tg_c < tg_p) ||
                (Prev == 2 && Curr == 4 && tg_c < tg_p) || (Prev == 4 && Curr == 2 && tg_c < tg_p))
            {
                longArc = true;
            }
        }

        // Jesli luk jest wiekszy niz 180 stopni
        if (longArc)
            AlphaArc = 360 - AlphaArc;

        // 9. Sprawdzenie czy jest pelny okrąg. Jesli tak to obliczamy pelny obwod okregu i kat 360 stopni
        if (abs(Xpre - Xcur) < tolerance && abs(Ypre - Ycur) < tolerance)
            AlphaArc = 360;

        // 10. Obliczenie dlugosci luku
        Arclong = AlphaArc / 180 * M_PI * Rarcradius;
    }

    return Arclong;
}

double PathCalculator::G17(double Xpre, double Ypre, double Xcur, double Ycur, double Rcur)
{
    //-- Obliczenia dlugosci luku w plaszczyznie G17 XYR --

    // 1. Obliczenie cieciwy luku AB

    // AB = sqrt(pow((Xpre - Xcur), 2) + pow((Ypre - Ycur), 2)); Obliczenie przeniesione do funkcji
    auto AB = MathUtils::CalcLineLength2d(Xpre, Ypre, Xcur, Ycur);

    // 2. Obliczenie polowy cieciwy
    auto ABhalf = (AB / 2);

    // 3. Obliczenie promienia R luku i w przypadku wartosci ujemnej (kat wiekszy niz 180 stopni) modul z liczby
    auto Rarcradius = abs(Rcur);

    // 4. Obliczenie polowy okregu dla danego luku
    auto Semicircle = (2 * M_PI * Rarcradius) / 2;

    // Warunek: jesli Semicircle = Rarcradius to luk ma 180stopni i dlugosc jego wynosi promien
    double tolerance = 1e-8;
    double Arclong{};
    if (abs(Semicircle - Rarcradius) < tolerance)
    {
        Arclong = Rarcradius;
    }
    else
    {
        // 6. Obliczenie wysokosci H
        auto Htriangle = sqrt(std::abs(pow(Rarcradius, 2) - pow(ABhalf, 2)));

        // 7. Obliczenie kata cieciwy AB i promienia R - UWAGA  ATAN2(a,b) = ATAN(b/a) z wyjatkiem gdy a=0
        auto AangleHR = atan(ABhalf / Htriangle) * Deg;

        // 8. Obliczenie kata calego luku
        auto AlphaArc = AangleHR * 2;

        // Jesli luk jest wiekszy niz 180 stopni
        bool longArc = Rcur < 0.0 ? true : false;
        if (longArc)
            AlphaArc = 360 - AlphaArc;

        // 9. Sprawdzenie czy jest pelny okrąg. Jesli tak to obliczamy pelna obwod okregu i kat 360 stopni

        if (abs(Xpre - Xcur) < tolerance && abs(Ypre - Ycur) < tolerance)
            AlphaArc = 360;

        // 10. Obliczenie dlugosci luku
        Arclong = AlphaArc / 180 * M_PI * Rarcradius;
    }

    return Arclong;
}

double PathCalculator::G17(double Xcur, double Ycur, double Icur, double Jcur, int Direction_CW_CCW)
{
    // Obliczenie cieciwy luku AB - wektora wypadkowego przemieszczenia narzedzia
    auto AB = MathUtils::CalcLineLength2d(Xcur, Ycur);

    // Obliczenie promienia R luku
    auto Rarcradius = MathUtils::CalcLineLength2d(Icur, Jcur);

    // Obliczenie polowy cieciwy
    auto ABhalf = (AB / 2);

    // Obliczenie polowy okregu dla danego luku
    auto Semicircle = (2 * M_PI * Rarcradius) / 2;

    // Warunek: jesli Semicircle = Rarcradius to luk ma 180stopni i dlugosc jego wynosi promien
    double tolerance = 1e-8;
    double Arclong{};
    if (abs(Semicircle - Rarcradius) < tolerance)
        Arclong = Rarcradius;

    // Sprawdzenie czy jest pelny okrąg. Jesli tak to obliczamy pelna obwod okregu i kat 360 stopni
    if (Xcur == 0 && Ycur == 0)
        Arclong = 2 * M_PI * Rarcradius;

    if (Arclong == 0)
    {
        // Obliczenie kata cieciwy AB i promienia R
        auto AangleHR = asin(ABhalf / Rarcradius) * Deg;

        // Obliczenie kata calego luku
        auto AlphaArc = AangleHR * 2;

        // Obliczenie dlugosci luku
        Arclong = AlphaArc / 360 * 2 * M_PI * Rarcradius;

        // Sprawdzenie czy luk jest wiekszy niz 180 stopni
        bool longArc{};
        if (Direction_CW_CCW == 3)
        {
            if ((Xcur > 0 && Ycur > 0 && Icur >= 0 && Jcur >= 0) || (Xcur > 0 && Ycur < 0 && Icur >= 0 && Jcur <= 0))
            {
                longArc = true;
            }
        }
        else if (Direction_CW_CCW == 2)
        {
            if ((Xcur < 0 && Ycur < 0 && Icur <= 0 && Jcur <= 0) || (Xcur < 0 && Ycur > 0 && Icur <= 0 && Jcur >= 0))
            {
                longArc = true;
            }
        }

        if (longArc)
            Arclong = 2 * M_PI * Rarcradius - Arclong;
    }

    return Arclong;
}

double PathCalculator::G17(double Xcur, double Ycur, double Rcur)
{
    const bool longArc = Rcur < 0.0 ? true : false;

    // Obliczenie cieciwy luku AB - wektora wypadkowego przemieszczenia narzedzia
    const auto AB = MathUtils::CalcLineLength2d(Xcur, Ycur);

    // Obliczenie promienia R luku
    const auto Rarcradius = abs(Rcur);

    // Obliczenie polowy cieciwy
    const auto ABhalf = (AB / 2);

    // Obliczenie polowy okregu dla danego luku
    const auto Semicircle = (2 * M_PI * Rarcradius) / 2;

    // Warunek: jesli Semicircle = Rarcradius to luk ma 180stopni i dlugosc jego wynosi promien
    const double tolerance = 1e-8;
    double       Arclong{};
    if (abs(Semicircle - Rarcradius) < tolerance)
        Arclong = Rarcradius;

    // Sprawdzenie czy jest pelny okrąg. Jesli tak to obliczamy pelna obwod okregu i kat 360 stopni
    if (Xcur == 0 && Ycur == 0)
        Arclong = 2 * M_PI * Rarcradius;

    if (Arclong == 0)
    {
        // Obliczenie kata cieciwy AB i promienia R
        const auto AangleHR = asin(ABhalf / Rarcradius) * Deg;

        // Obliczenie kata calego luku
        const auto AlphaArc = AangleHR * 2;

        // Obliczenie dlugosci luku
        Arclong = AlphaArc / 360 * 2 * M_PI * Rarcradius;

        if (longArc)
            Arclong = 2 * M_PI * Rarcradius - Arclong;
    }

    return Arclong;
}

double PathCalculator::G2G3_HelixG17(double Zpre, double Zcur, double Icur, double Jcur)
{
    const auto HPitch = Zpre - Zcur;
    return HelixG17(HPitch, Icur, Jcur);
}

double PathCalculator::G2G3_HelixG17(double Zcur, double Icur, double Jcur)
{
    const auto HPitch = Zcur;
    return HelixG17(HPitch, Icur, Jcur);
}

double PathCalculator::HelixG17(double HPitch, double Icur, double Jcur)
{
    const auto HRadius = MathUtils::CalcLineLength2d(Icur, Jcur);
    const auto HLength = MathUtils::CalcCircleLength2d(HRadius, HPitch);
    return HLength;
}

double PathCalculator::G2G3_HelixG17_Quarter(double Zpre, double Zcur, double Icur, double Jcur)
{
    const auto HPitch = (Zpre - Zcur) * 4.;
    return HelixG17_Quarter(HPitch, Icur, Jcur);
}

double PathCalculator::G2G3_HelixG17_Quarter(double Zcur, double Icur, double Jcur)
{
    const auto HPitch = Zcur * 4.;
    return HelixG17_Quarter(HPitch, Icur, Jcur);
}

double PathCalculator::HelixG17_Quarter(double HPitch, double Icur, double Jcur)
{
    const auto HRadius = MathUtils::CalcLineLength2d(Icur, Jcur);
    const auto HLength = MathUtils::CalcCircleLength2d(HRadius, HPitch) / 4.;
    return HLength;
}

double PathCalculator::G2G3_HelixG17R(double Zpre, double Zcur, double R)
{
    const auto HPitch = Zpre - Zcur;
    return HelixG17R(HPitch, R);
}

double PathCalculator::G2G3_HelixG17R(double Zcur, double R)
{
    const auto HPitch = Zcur;
    return HelixG17R(HPitch, R);
}

double PathCalculator::HelixG17R(double HPitch, double R)
{
    const auto HLength = MathUtils::CalcCircleLength2d(R, HPitch);
    return HLength;
}

double PathCalculator::G2G3_HelixG17_QuarterR(double Zpre, double Zcur, double R)
{
    const auto HPitch = (Zpre - Zcur) * 4.;
    return HelixG17_QuarterR(HPitch, R);
}

double PathCalculator::G2G3_HelixG17_QuarterR(double Zcur, double R)
{
    const auto HPitch = Zcur * 4.;
    return HelixG17_QuarterR(HPitch, R);
}

double PathCalculator::HelixG17_QuarterR(double HPitch, double R)
{
    const auto HLength = MathUtils::CalcCircleLength2d(R, HPitch) / 4.;
    return HLength;
}

double PathCalculator::G2G3_HelixLast(double HLength)
{
    return HLength / 2;
}

double PathCalculator::G2G3_HelixG17_ArchimedesLparam(double Icur, double Jcur, double Lparam)
{
    const auto HRadius = MathUtils::CalcLineLength2d(Icur, Jcur);
    return G2G3_Helix_Archimedes(Lparam, HRadius);
}

double PathCalculator::G2G3_HelixG17_ArchimedesQparam(double Icur, double Jcur, double Qparam)
{
    const auto HRadius = MathUtils::CalcLineLength2d(Icur, Jcur);
    const auto Lparam  = HRadius / Qparam;
    return G2G3_Helix_Archimedes(Lparam, HRadius);
}

double PathCalculator::G2G3_Helix_Archimedes(double Lparam, double HRadius)
{
    const auto aparam   = HRadius / (Lparam * 2 * M_PI);
    const auto LArchmds = aparam / 2 *
                          (Lparam * 2 * M_PI * std::sqrt(1 + std::pow(Lparam * 2 * M_PI, 2)) +
                           std::log(Lparam * 2 * M_PI + std::sqrt(1 + std::pow(Lparam * 2 * M_PI, 2))));
    return LArchmds;
}

std::pair<double, double> PathCalculator::DrillCyclePath_G17(double Zpre, double Zgleb, double Rparam,
                                                             EDepthProgrammingType depthProgrammingType,
                                                             EDrillGmode           Gmode,
                                                             EDrillGreturnMode GreturnMode /*= EDrillGreturnMode::G98*/)
{
    //-- Obliczenia dlugosci drogi w plaszczyznie G17 XY dla cyklu wiertarskiego --
    if (Gmode == EDrillGmode::G81 || Gmode == EDrillGmode::G82 || Gmode == EDrillGmode::G85 ||
        Gmode == EDrillGmode::G86 || Gmode == EDrillGmode::G88 || Gmode == EDrillGmode::G89)
    {
        if (depthProgrammingType == EDepthProgrammingType::Incremental)
        {
            if (GreturnMode == EDrillGreturnMode::G98)
            {
                auto Cycle_distance_Rapid  = Zpre - Rparam + std::abs(Zgleb);
                auto Cycle_distance_Linear = 2 * std::abs(Zgleb) - Cycle_distance_Rapid;
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
            else if (GreturnMode == EDrillGreturnMode::G99)
            {
                auto Cycle_distance_Rapid  = Zpre - Rparam + Zgleb;
                auto Cycle_distance_Linear = Zgleb;
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
        }
        else
        {
            if (GreturnMode == EDrillGreturnMode::G98)
            {
                /*if(Zpre == Zgleb)// check doubles using substraction, this check can be useful to warn user that is
                doing something stupid
                {
                    return std::make_pair(0.,0.); // this probably should be replaced by normal calculation below
                }
                else */
                if (Zpre > 0 && Zgleb < 0)
                {
                    auto Cycle_distance_Linear = Rparam + std::abs(Zgleb);
                    auto Cycle_distance_Rapid  = std::abs(Zpre) - std::abs(Rparam) + std::abs(Zpre) + std::abs(Zgleb);
                    return std::make_pair(std::abs(Cycle_distance_Linear), std::abs(Cycle_distance_Rapid));
                }
                else
                {
                    auto Cycle_distance_Linear = std::abs(Rparam) - std::abs(Zgleb);
                    auto Cycle_distance_Rapid  = std::abs(Zpre) - std::abs(Rparam) + std::abs(Zpre) - std::abs(Zgleb);
                    return std::make_pair(std::abs(Cycle_distance_Linear), std::abs(Cycle_distance_Rapid));
                }
            }
            else if (GreturnMode == EDrillGreturnMode::G99)
            {
                /*if(Zpre == Zgleb)
                {
                    return std::make_pair(0.,0.);
                }
                else */
                if (Zpre > 0 && Zgleb < 0)
                {
                    auto Cycle_distance_Linear = Rparam + std::abs(Zgleb);
                    auto Cycle_distance_Rapid  = std::abs(Zpre) + std::abs(Zgleb);
                    return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
                }
                else
                {
                    auto Cycle_distance_Linear = std::abs(Rparam) - std::abs(Zgleb);
                    auto Cycle_distance_Rapid  = std::abs(Zpre) - std::abs(Zgleb);
                    return std::make_pair(std::abs(Cycle_distance_Linear), std::abs(Cycle_distance_Rapid));
                }
            }
        }
    }
    else if (Gmode == EDrillGmode::G74 || Gmode == EDrillGmode::G84)
    {
        if (depthProgrammingType == EDepthProgrammingType::Incremental)
        {
            auto Cycle_distance_Linear = 2 * std::abs(Zgleb);
            if (GreturnMode == EDrillGreturnMode::G98)
            {
                auto Cycle_distance_Rapid = 2 * std::abs(Rparam - Zpre);
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
            else if (GreturnMode == EDrillGreturnMode::G99)
            {
                auto Cycle_distance_Rapid = std::abs(Rparam - Zpre);
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
        }
        else
        {
            auto Cycle_distance_Linear = 2 * std::abs(Zgleb - Rparam);
            if (GreturnMode == EDrillGreturnMode::G98)
            {
                auto Cycle_distance_Rapid = 2 * std::abs(Rparam - Zpre);
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
            else if (GreturnMode == EDrillGreturnMode::G99)
            {
                auto Cycle_distance_Rapid = std::abs(Rparam - Zpre);
                return std::make_pair(std::abs(Cycle_distance_Linear), std::abs(Cycle_distance_Rapid));
            }
        }
    }

    return std::make_pair(0., 0.);
}

std::pair<double, double> PathCalculator::DrillCyclePath_G17(double Zpre, double Zgleb, double Rparam, double Qparam,
                                                             EDepthProgrammingType depthProgrammingType,
                                                             EDrillGmode           Gmode,
                                                             EDrillGreturnMode GreturnMode /*= EDrillGreturnMode::G98*/)
{
    //-- Obliczenia dlugosci drogi w plaszczyznie G17 XY dla cyklu wiertarskiego --
    if (Gmode == EDrillGmode::G87)
    {
        // Dla tego ruchu - wytaczanie wsteczne - jest tylko G98 i EPosition::Absolute
        auto Cycle_distance_Rapid  = 2 * std::abs(Rparam - Zpre) + 4 * Qparam - std::abs(Rparam - Zgleb);
        auto Cycle_distance_Linear = std::abs(Rparam - Zgleb);
        return std::make_pair(std::abs(Cycle_distance_Linear), std::abs(Cycle_distance_Rapid));
    }
    else if (Gmode == EDrillGmode::G76)
    {
        if (depthProgrammingType == EDepthProgrammingType::Incremental)
        {
            auto Cycle_distance_Linear = std::abs(Zgleb);
            if (GreturnMode == EDrillGreturnMode::G98)
            {
                auto Cycle_distance_Rapid = 2 * std::abs(Rparam - Zpre) + std::abs(Zgleb) + 2 * Qparam;
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
            else if (GreturnMode == EDrillGreturnMode::G99)
            {
                auto Cycle_distance_Rapid = std::abs(Rparam - Zpre) + std::abs(Zgleb) + 2 * Qparam;
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
        }
        else
        {
            auto Cycle_distance_Linear = std::abs(Zgleb - Rparam);
            if (GreturnMode == EDrillGreturnMode::G98)
            {
                auto Cycle_distance_Rapid = std::abs(Rparam - Zpre) + std::abs(Zgleb - Zpre) + 2 * Qparam;
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
            else if (GreturnMode == EDrillGreturnMode::G99)
            {
                auto Cycle_distance_Rapid = std::abs(Rparam - Zpre) + std::abs(Zgleb - Rparam) + 2 * Qparam;
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
        }
    }
    else if (Gmode == EDrillGmode::G73)
    {
        auto Cycle_distance_Linear = 0.0;
        if (depthProgrammingType == EDepthProgrammingType::Incremental)
        {
            auto Gleb_rzeczyw     = std::abs(Zgleb);
            auto liczbaprzejsc    = std::floor(Gleb_rzeczyw / Qparam);
            auto dparam           = Qparam / 5;
            Cycle_distance_Linear = Gleb_rzeczyw + dparam * liczbaprzejsc;
        }
        else
        {
            auto Gleb_rzeczyw     = std::abs(Rparam - Zgleb);
            auto liczbaprzejsc    = std::floor(Gleb_rzeczyw / Qparam);
            auto dparam           = Qparam / 5;
            Cycle_distance_Linear = Gleb_rzeczyw + dparam * liczbaprzejsc;
        }

        if (GreturnMode == EDrillGreturnMode::G98)
        {
            if (depthProgrammingType == EDepthProgrammingType::Incremental)
            {
                auto Cycle_distance_Rapid = 2 * std::abs(Rparam - Zpre) + std::abs(Zgleb);
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
            else
            {
                auto Cycle_distance_Rapid = std::abs(Rparam - Zpre) * 2 + std::abs(Zgleb - Rparam);
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
        }
        else if (GreturnMode == EDrillGreturnMode::G99)
        {
            if (depthProgrammingType == EDepthProgrammingType::Incremental)
            {
                auto Cycle_distance_Rapid = std::abs(Rparam - Zpre) + std::abs(Zgleb);
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
            else
            {
                auto Cycle_distance_Rapid = std::abs(Rparam - Zpre) + std::abs(Zgleb - Rparam);
                return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            }
        }
    }
    else if (Gmode == EDrillGmode::G83)
    {
        int mnoznikG98G99 = GreturnMode == EDrillGreturnMode::G99 ? 1 : 2;
        if (depthProgrammingType == EDepthProgrammingType::Incremental)
        {
            auto Gleb_rzeczyw          = Zgleb;
            auto liczbaprzejsc         = std::floor(Gleb_rzeczyw / Qparam);
            auto resztagleb            = std::fmod(Gleb_rzeczyw, Qparam);
            auto dparam                = Qparam / 5;
            auto Cycle_distance_Linear = Qparam * liczbaprzejsc + dparam * liczbaprzejsc + resztagleb;
            auto sumaQ                 = liczbaprzejsc * liczbaprzejsc + liczbaprzejsc;
            int  liczbaQ               = static_cast<int>(sumaQ);
            auto Cycle_distance        = liczbaQ * Qparam + mnoznikG98G99 * std::abs(Zpre - Rparam);
            if (resztagleb > 0)
            {
                Cycle_distance = Cycle_distance + 2 * Gleb_rzeczyw;
            }
            auto Cycle_distance_Rapid = Cycle_distance - Cycle_distance_Linear;
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
        else
        {
            /*if (Zpre == Zgleb)
            {
                return std::make_pair(0.,0.);
            }
            else
            {*/
            auto Gleb_rzeczyw          = std::abs(Rparam - Zgleb);
            auto liczbaprzejsc         = std::floor(Gleb_rzeczyw / Qparam);
            auto resztagleb            = std::fmod(Gleb_rzeczyw, Qparam);
            auto dparam                = Qparam / 5;
            auto Cycle_distance_Linear = Qparam * liczbaprzejsc + dparam * liczbaprzejsc + resztagleb;
            auto sumaQ                 = liczbaprzejsc * liczbaprzejsc + liczbaprzejsc;
            int  liczbaQ               = static_cast<int>(sumaQ);
            auto Cycle_distance        = liczbaQ * Qparam + mnoznikG98G99 * std::abs(Zpre - Rparam);
            if (resztagleb > 0)
            {
                Cycle_distance = Cycle_distance + 2 * Gleb_rzeczyw;
            }
            auto Cycle_distance_Rapid = Cycle_distance - Cycle_distance_Linear;
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
            //}
        }
    }

    return std::make_pair(0., 0.);
}

std::pair<double, double> PathCalculator::DrillCyclePath_G17(double Zgleb, double Rparam, EDrillGmode Gmode,
                                                             EDrillGreturnMode GreturnMode /*= EDrillGreturnMode::G98*/)
{
    //-- Obliczenia dlugosci drogi w plaszczyznie G17 XY dla cyklu wiertarskiego --
    if (Gmode == EDrillGmode::G81 || Gmode == EDrillGmode::G82 || Gmode == EDrillGmode::G85 ||
        Gmode == EDrillGmode::G86 || Gmode == EDrillGmode::G88 || Gmode == EDrillGmode::G89)
    {
        const auto Cycle_distance_Linear = std::abs(Zgleb);
        if (GreturnMode == EDrillGreturnMode::G98)
        {
            const auto Cycle_distance_Rapid = std::abs(Zgleb) + 2 * std::abs(Rparam);
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
        else if (GreturnMode == EDrillGreturnMode::G99)
        {
            const auto Cycle_distance_Rapid = std::abs(Zgleb) + std::abs(Rparam);
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
    }
    else if (Gmode == EDrillGmode::G74 || Gmode == EDrillGmode::G84)
    {
        const auto Cycle_distance_Linear = 2 * std::abs(Zgleb);
        if (GreturnMode == EDrillGreturnMode::G98)
        {
            const auto Cycle_distance_Rapid = 2 * std::abs(Rparam);
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
        else if (GreturnMode == EDrillGreturnMode::G99)
        {
            const auto Cycle_distance_Rapid = 0;
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
    }

    return std::make_pair(0., 0.);
}

std::pair<double, double> PathCalculator::DrillCyclePath_G17(double Zgleb, double Rparam, double Qparam,
                                                             EDrillGmode       Gmode,
                                                             EDrillGreturnMode GreturnMode /*= EDrillGreturnMode::G98*/)
{
    //-- Obliczenia dlugosci drogi w plaszczyznie G17 XY dla cyklu wiertarskiego --
    if (Gmode == EDrillGmode::G87)
    {
        // wytaczanie wsteczne
        const auto Cycle_distance_Rapid  = (2 * std::abs(Rparam)) + (3 * Qparam) + sqrt(pow(Qparam, 2) + pow(Zgleb, 2));
        const auto Cycle_distance_Linear = std::abs(Zgleb);
        return std::make_pair(Cycle_distance_Linear, std::abs(Cycle_distance_Rapid));
    }
    else if (Gmode == EDrillGmode::G76)
    {
        const auto Cycle_distance_Linear = std::abs(Zgleb);
        if (GreturnMode == EDrillGreturnMode::G98)
        {
            const auto Cycle_distance_Rapid = std::abs(Rparam) + std::abs(Zgleb) + 2 * Qparam;
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
        else if (GreturnMode == EDrillGreturnMode::G99)
        {
            const auto Cycle_distance_Rapid = std::abs(Zgleb) + 2 * Qparam;
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
    }
    else if (Gmode == EDrillGmode::G73)
    {
        const auto Gleb_rzeczyw          = std::abs(Zgleb);
        const auto liczbaprzejsc         = std::floor(Gleb_rzeczyw / Qparam);
        const auto dparam                = Qparam / 5;
        const auto Cycle_distance_Linear = Gleb_rzeczyw + dparam * liczbaprzejsc;

        if (GreturnMode == EDrillGreturnMode::G98)
        {
            auto Cycle_distance_Rapid = 2 * std::abs(Rparam) + std::abs(Zgleb) + (dparam * liczbaprzejsc);
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
        else if (GreturnMode == EDrillGreturnMode::G99)
        {
            auto Cycle_distance_Rapid = std::abs(Rparam) + std::abs(Zgleb) + (dparam * liczbaprzejsc);
            return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
        }
    }
    else if (Gmode == EDrillGmode::G83)
    {
        const int  mnoznikG98G99         = GreturnMode == EDrillGreturnMode::G99 ? 1 : 2;
        const auto Gleb_rzeczyw          = std::abs(Zgleb);
        const auto liczbaprzejsc         = std::floor(Gleb_rzeczyw / Qparam);
        const auto resztagleb            = std::fmod(Gleb_rzeczyw, Qparam);
        const auto dparam                = Qparam / 5;
        const auto Cycle_distance_Linear = Gleb_rzeczyw + dparam * liczbaprzejsc;
        const auto sumaQ                 = liczbaprzejsc * liczbaprzejsc + liczbaprzejsc;
        const int  liczbaQ               = static_cast<int>(sumaQ);
        auto       Cycle_distance        = liczbaQ * Qparam + mnoznikG98G99 * std::abs(Rparam);
        if (resztagleb > 0)
            Cycle_distance = Cycle_distance + 2 * Gleb_rzeczyw;
        const auto Cycle_distance_Rapid = Cycle_distance - Cycle_distance_Linear;
        return std::make_pair(Cycle_distance_Linear, Cycle_distance_Rapid);
    }

    return std::make_pair(0., 0.);
}

std::pair<double, double> PathCalculator::DrillCyclePathHole_G17(double Xpre, double Ypre, double Xcur, double Ycur,
                                                                 const std::pair<double, double>& Cycle_distance)
{
    // Ponizej jest algorytm pozwalajacy obliczyc droge pomiedzy otworami. Wlasciwy algorytm musi byc dostosowany
    // do odczytanych wartosci z kodu NC, czyli cykl wiertarski odbywa sie pomiedzy blokami z G8x, a G80
    auto Cycle_distance_between_holes = MathUtils::CalcLineLength2d(Xpre, Ypre, Xcur, Ycur);
    return std::make_pair(Cycle_distance.first, Cycle_distance.second + Cycle_distance_between_holes);
}

std::pair<double, double> PathCalculator::DrillCyclePathHole_G17(double X, double Y,
                                                                 const std::pair<double, double>& Cycle_distance)
{
    // Ponizej jest algorytm pozwalajacy obliczyc droge pomiedzy otworami. Wlasciwy algorytm musi byc dostosowany
    // do odczytanych wartosci z kodu NC, czyli cykl wiertarski odbywa sie pomiedzy blokami z G8x, a G80
    auto Cycle_distance_between_holes = MathUtils::CalcLineLength2d(X, Y);
    return std::make_pair(Cycle_distance.first, Cycle_distance.second + Cycle_distance_between_holes);
}

} // namespace fanuc
} // namespace parser
