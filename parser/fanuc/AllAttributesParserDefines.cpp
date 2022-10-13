#include "stdafx.h"

#include "AllAttributesParserDefines.h"

#include <iostream>
#include <sstream>

#include <boost/optional/optional_io.hpp>

namespace parser {
namespace fanuc {

std::ostream& operator<<(std::ostream& os, const StringAttributeData& data)
{
    os << "word:" << data.word << " value:" << data.value;
    return os;
}

std::ostream& operator<<(std::ostream& os, const CharAttributeData& data)
{
    os << "word:" << data.word << " value:" << data.value;
    return os;
}

std::ostream& operator<<(std::ostream& os, const DecimalAttributeData& data)
{
    os << "word:" << data.word << " assign:" << data.assign << " sign:" << data.sign
       << " open_bracket:" << data.open_bracket << " macro:" << data.macro << " value:" << data.value
       << " dot:" << data.dot << " value2:" << data.value2 << " close_bracket:" << data.close_bracket;

    return os;
}

std::string to_string(WordType e)
{
    switch (e)
    {
    case WordType::misc:
        return "misc";
    case WordType::angle:
        return "angle";
    case WordType::calc:
        return "calc";
    case WordType::code:
        return "code";
    case WordType::feed:
        return "feed";
    case WordType::length:
        return "length";
    case WordType::macro:
        return "macro";
    case WordType::numbering:
        return "numbering";
    case WordType::assignable:
        return "assignable";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported WordType value");
}

WordType to_WordType(const std::string& s)
{
    if (s == "misc")
        return WordType::misc;
    else if (s == "angle")
        return WordType::angle;
    else if (s == "calc")
        return WordType::calc;
    else if (s == "code")
        return WordType::code;
    else if (s == "feed")
        return WordType::feed;
    else if (s == "length")
        return WordType::length;
    else if (s == "macro")
        return WordType::macro;
    else if (s == "numbering")
        return WordType::numbering;
    else if (s == "assignable")
        return WordType::assignable;
    else
        throw std::invalid_argument("Unsupported WordType value: " + s);
}

LEFT_STREAM_OP(WordType)
RIGHT_STREAM_OP(WordType)

} // namespace fanuc
} // namespace parser
