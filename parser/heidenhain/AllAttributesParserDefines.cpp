#include "stdafx.h"

#include "AllAttributesParserDefines.h"

#include <iostream>
#include <sstream>

#include <boost/optional/optional_io.hpp>

namespace parser {
namespace heidenhain {

std::string to_string(EBeginEnd e)
{
    switch (e)
    {
    case EBeginEnd::begin:
        return "begin";
    case EBeginEnd::end:
        return "end";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported EBeginEnd value");
}

EBeginEnd to_EBeginEnd(const std::string& s)
{
    if (s == "begin")
        return EBeginEnd::begin;
    else if (s == "end")
        return EBeginEnd::end;
    else
        throw std::invalid_argument("Unsupported EBeginEnd value: " + s);
}

LEFT_STREAM_OP(EBeginEnd)
RIGHT_STREAM_OP(EBeginEnd)

std::string to_string(EUnit e)
{
    switch (e)
    {
    case EUnit::mm:
        return "mm";
    case EUnit::inch:
        return "inch";
        // omit default case to trigger compiler warning for missing cases
    }

    throw std::invalid_argument("Unsupported EUnit value");
}

EUnit to_EUnit(const std::string& s)
{
    if (s == "mm")
        return EUnit::mm;
    else if (s == "inch")
        return EUnit::inch;
    else
        throw std::invalid_argument("Unsupported EUnit value: " + s);
}

LEFT_STREAM_OP(EUnit)
RIGHT_STREAM_OP(EUnit)

std::ostream& operator<<(std::ostream& os, const ProgramBeginEndData& data)
{
    os << "begin_end:" << data.begin_end << " name:" << data.name << " unit:" << data.unit;
    return os;
}

std::ostream& operator<<(std::ostream& os, const CommentData& data)
{
    os << "semicolon:" << data.semicolon << " value:" << data.value << " tilde:" << data.tilde;
    return os;
}

std::ostream& operator<<(std::ostream& os, const LineNumberData& data)
{
    os << "value:" << data.value;
    return os;
}

std::ostream& operator<<(std::ostream& os, const CycleDef& data)
{
    os << "word1:" << data.word1 << "word2:" << data.word2 << " value:" << data.value << " dot:" << data.dot
       << " value2:" << data.value2;
    return os;
}

std::ostream& operator<<(std::ostream& os, const CycleParam& data)
{
    os << "name:" << data.name << " id:" << data.id << " assign:" << data.assign << " sign:" << data.sign
       << " value:" << data.value << " dot:" << data.dot << " value2:" << data.value2;
    return os;
}

std::ostream& operator<<(std::ostream& os, const GeneralAttributeData& data)
{
    os << "word:" << data.word << " sign:" << data.sign << " name:" << data.name << " value:" << data.value
       << " dot:" << data.dot << " value2:" << data.value2 << " tilde:" << data.tilde;

    return os;
}

} // namespace heidenhain
} // namespace parser
