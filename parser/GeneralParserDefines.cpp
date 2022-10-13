﻿#include "stdafx.h"

#include "GeneralParserDefines.h"

#include <iostream>
#include <sstream>

#include "Macro.h"

namespace parser {

std::string to_string(ECncType e)
{
    switch (e)
    {
    case ECncType::Fanuc:
        return "fanuc";
    case ECncType::Generic:
        return "generic";
    case ECncType::Haas:
        return "haas";
    case ECncType::Heidenhain:
        return "heidenhain";
    case ECncType::Makino:
        return "makino";
        // omit default case to trigger compiler warning for missing cases
    }
    throw std::invalid_argument("Unsupported ECncType value");
}

ECncType to_ECncType(const std::string& s)
{
    if (s == "fanuc")
        return ECncType::Fanuc;
    else if (s == "generic")
        return ECncType::Generic;
    else if (s == "haas")
        return ECncType::Haas;
    else if (s == "heidenhain")
        return ECncType::Heidenhain;
    else if (s == "makino")
        return ECncType::Makino;
    else
        throw std::invalid_argument("Unsupported ECncType value: " + s);
}

LEFT_STREAM_OP(ECncType)
RIGHT_STREAM_OP(ECncType)

std::string to_string(EFanucParserType e)
{
    switch (e)
    {
    case EFanucParserType::FanucLathe:
        return "fanuc_lathe";
    case EFanucParserType::FanucMill:
        return "fanuc_mill";
    case EFanucParserType::GenericLathe:
        return "generic_lathe";
    case EFanucParserType::GenericMill:
        return "generic_mill";
    case EFanucParserType::HaasLathe:
        return "haas_lathe";
    case EFanucParserType::HaasMill:
        return "haas_mill";
    case EFanucParserType::MakinoMill:
        return "makino_mill";
        // omit default case to trigger compiler warning for missing cases
    }
    throw std::invalid_argument("Unsupported EFanucParserType value");
}

EFanucParserType to_EFanucParserType(const std::string& s)
{
    if (s == "fanuc_lathe")
        return EFanucParserType::FanucLathe;
    else if (s == "fanuc_mill")
        return EFanucParserType::FanucMill;
    if (s == "generic_lathe")
        return EFanucParserType::GenericLathe;
    else if (s == "generic_mill")
        return EFanucParserType::GenericMill;
    else if (s == "haas_lathe")
        return EFanucParserType::HaasLathe;
    else if (s == "haas_mill")
        return EFanucParserType::HaasMill;
    else if (s == "makino_mill")
        return EFanucParserType::MakinoMill;
    else
        throw std::invalid_argument("Unsupported EFanucParserType value: " + s);
}

LEFT_STREAM_OP(EFanucParserType)
RIGHT_STREAM_OP(EFanucParserType)

std::string to_string(EMachineToolType e)
{
    switch (e)
    {
    case EMachineToolType::Mill:
        return "mill";
    case EMachineToolType::Lathe:
        return "lathe";
    case EMachineToolType::Millturn:
        return "millturn";
        // omit default case to trigger compiler warning for missing cases
    }
    throw std::invalid_argument("Unsupported EMachineToolType value");
}

EMachineToolType to_EMachineToolType(const std::string& s)
{
    if (s == "mill")
        return EMachineToolType::Mill;
    else if (s == "lathe")
        return EMachineToolType::Lathe;
    else if (s == "millturn")
        return EMachineToolType::Millturn;
    else
        throw std::invalid_argument("Unsupported EMachineToolType value: " + s);
}

LEFT_STREAM_OP(EMachineToolType)
RIGHT_STREAM_OP(EMachineToolType)

std::string to_string(ELanguage e)
{
    switch (e)
    {
    case ELanguage::English:
        return "English";
    case ELanguage::Polish:
        return "Polish";
        // omit default case to trigger compiler warning for missing cases
    }
    throw std::invalid_argument("Unsupported ELanguage value");
}

ELanguage to_ELanguage(const std::string& s)
{
    if (s == "English")
        return ELanguage::English;
    else if (s == "Polish")
        return ELanguage::Polish;
    else
        throw std::invalid_argument("Unsupported ELanguage value: " + s);
}

LEFT_STREAM_OP(ELanguage)
RIGHT_STREAM_OP(ELanguage)

std::string to_string(EDriverUnits e)
{
    switch (e)
    {
    case EDriverUnits::Millimeter:
        return "millimeter";
    case EDriverUnits::Inch:
        return "inch";
        // omit default case to trigger compiler warning for missing cases
    }
    throw std::invalid_argument("Unsupported EDriverUnits value");
}

EDriverUnits to_EDriverUnits(const std::string& s)
{
    if (s == "millimeter")
        return EDriverUnits::Millimeter;
    else if (s == "inch")
        return EDriverUnits::Inch;
    else
        throw std::invalid_argument("Unsupported EDriverUnits value: " + s);
}

LEFT_STREAM_OP(EDriverUnits)
RIGHT_STREAM_OP(EDriverUnits)

std::string to_string(UnitConversionType e)
{
    switch (e)
    {
    case UnitConversionType::metric_to_imperial:
        return "m2i";
    case UnitConversionType::imperial_to_metric:
        return "i2m";
        // omit default case to trigger compiler warning for missing cases
    }
    throw std::invalid_argument("Unsupported UnitConversionType value");
}

UnitConversionType to_UnitConversionType(const std::string& s)
{
    if (s == "m2i")
        return UnitConversionType::metric_to_imperial;
    else if (s == "i2m")
        return UnitConversionType::imperial_to_metric;
    else
        throw std::invalid_argument("Unsupported UnitConversionType value: " + s);
}

LEFT_STREAM_OP(UnitConversionType)
RIGHT_STREAM_OP(UnitConversionType)

std::string to_string(AxesRotatingOption e)
{
    switch (e)
    {
    case AxesRotatingOption::Xrotate90degrees:
        return "X90";
    case AxesRotatingOption::Xrotate180degrees:
        return "X180";
    case AxesRotatingOption::Xrotate270degrees:
        return "X270";
    case AxesRotatingOption::Yrotate90degrees:
        return "Y90";
    case AxesRotatingOption::Yrotate180degrees:
        return "Y180";
    case AxesRotatingOption::Yrotate270degrees:
        return "Y270";
    case AxesRotatingOption::Zrotate90degrees:
        return "Z90";
    case AxesRotatingOption::Zrotate180degrees:
        return "Z180";
    case AxesRotatingOption::Zrotate270degrees:
        return "Z270";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported AxesRotatingOption value");
}

AxesRotatingOption to_AxesRotatingOption(const std::string& s)
{
    if (s == "X90")
        return AxesRotatingOption::Xrotate90degrees;
    else if (s == "X180")
        return AxesRotatingOption::Xrotate180degrees;
    else if (s == "X270")
        return AxesRotatingOption::Xrotate270degrees;
    else if (s == "Y90")
        return AxesRotatingOption::Yrotate90degrees;
    else if (s == "Y180")
        return AxesRotatingOption::Yrotate180degrees;
    else if (s == "Y270")
        return AxesRotatingOption::Yrotate270degrees;
    else if (s == "Z90")
        return AxesRotatingOption::Zrotate90degrees;
    else if (s == "Z180")
        return AxesRotatingOption::Zrotate180degrees;
    else if (s == "Z270")
        return AxesRotatingOption::Zrotate270degrees;
    else
        throw std::invalid_argument("Unsupported AxesRotatingOption value: " + s);
}

LEFT_STREAM_OP(AxesRotatingOption)
RIGHT_STREAM_OP(AxesRotatingOption)

std::string to_string(EMotion e)
{
    switch (e)
    {
    case EMotion::RapidTraverse:
        return "rapid_traverse";
    case EMotion::LinearInterpolation:
        return "linear_interpolation";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported EMotion value");
}

EMotion to_EMotion(const std::string& s)
{
    if (s == "rapid_traverse")
        return EMotion::RapidTraverse;
    else if (s == "linear_interpolation")
        return EMotion::LinearInterpolation;
    else
        throw std::invalid_argument("Unsupported EMotion value: " + s);
}

LEFT_STREAM_OP(EMotion)
RIGHT_STREAM_OP(EMotion)

std::string to_string(EWorkPlane e)
{
    switch (e)
    {
    case EWorkPlane::XY:
        return "XY";
    case EWorkPlane::XZ:
        return "XZ";
    case EWorkPlane::YZ:
        return "YZ";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported EWorkPlane value");
}

EWorkPlane to_EWorkPlane(const std::string& s)
{
    if (s == "XY")
        return EWorkPlane::XY;
    else if (s == "XZ")
        return EWorkPlane::XZ;
    else if (s == "YZ")
        return EWorkPlane::YZ;
    else
        throw std::invalid_argument("Unsupported EWorkPlane value: " + s);
}

LEFT_STREAM_OP(EWorkPlane)
RIGHT_STREAM_OP(EWorkPlane)

std::string to_string(EProgrammingType e)
{
    switch (e)
    {
    case EProgrammingType::Absolute:
        return "absolute";
    case EProgrammingType::Incremental:
        return "incremental";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported EProgrammingType value");
}

EProgrammingType to_EProgrammingType(const std::string& s)
{
    if (s == "absolute")
        return EProgrammingType::Absolute;
    else if (s == "incremental")
        return EProgrammingType::Incremental;
    else
        throw std::invalid_argument("Unsupported EProgrammingType value: " + s);
}

LEFT_STREAM_OP(EProgrammingType)
RIGHT_STREAM_OP(EProgrammingType)

std::string to_string(EFeedMode e)
{
    switch (e)
    {
    case EFeedMode::PerMinute:
        return "feed_per_minute";
    case EFeedMode::PerRevolution:
        return "feed_per_revolution";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported EFeedMode value");
}

EFeedMode to_EFeedMode(const std::string& s)
{
    if (s == "feed_per_minute")
        return EFeedMode::PerMinute;
    else if (s == "feed_per_revolution")
        return EFeedMode::PerRevolution;
    else
        throw std::invalid_argument("Unsupported EFeedMode value: " + s);
}

LEFT_STREAM_OP(EFeedMode)
RIGHT_STREAM_OP(EFeedMode)

std::string to_string(ERotationDirection e)
{
    switch (e)
    {
    case ERotationDirection::Right:
        return "right (M03) CLW (clockwise)";
    case ERotationDirection::Left:
        return "left (M04) CCLW (counterclockwise)";
    case ERotationDirection::Stop:
        return "stop (M05)";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported ERotationDirection value");
}

ERotationDirection to_ERotationDirection(const std::string& s)
{
    if (s == "right" || s == "M03" || s == "CLW")
        return ERotationDirection::Right;
    else if (s == "left" || s == "M04" || s == "CCLW")
        return ERotationDirection::Left;
    else if (s == "stop" || s == "M05")
        return ERotationDirection::Stop;
    else
        throw std::invalid_argument("Unsupported ERotationDirection value: " + s);
}

LEFT_STREAM_OP(ERotationDirection)
RIGHT_STREAM_OP(ERotationDirection)

std::string to_string(EDepthProgrammingType e)
{
    switch (e)
    {
    case EDepthProgrammingType::Absolute:
        return "absolute";
    case EDepthProgrammingType::Incremental:
        return "incremental";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported EDepthProgrammingType value");
}

EDepthProgrammingType to_EDepthProgrammingType(const std::string& s)
{
    if (s == "absolute")
        return EDepthProgrammingType::Absolute;
    else if (s == "incremental")
        return EDepthProgrammingType::Incremental;
    else
        throw std::invalid_argument("Unsupported EDepthProgrammingType value: " + s);
}

LEFT_STREAM_OP(EDepthProgrammingType)
RIGHT_STREAM_OP(EDepthProgrammingType)

std::string to_string(EDrillGreturnMode e)
{
    switch (e)
    {
    case EDrillGreturnMode::G98:
        return "beginning";
    case EDrillGreturnMode::G99:
        return "point_r";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported EDrillGreturnMode value");
}

EDrillGreturnMode to_EDrillGreturnMode(const std::string& s)
{
    if (s == "beginning")
        return EDrillGreturnMode::G98;
    else if (s == "point_r")
        return EDrillGreturnMode::G99;
    else
        throw std::invalid_argument("Unsupported EDrillGreturnMode value: " + s);
}

LEFT_STREAM_OP(EDrillGreturnMode)
RIGHT_STREAM_OP(EDrillGreturnMode)

} // namespace parser
