#pragma once

#include "parser_export.h"

#include <map>
#include <string>
#include <tuple>
#include <unordered_map>

#include <boost/optional/optional.hpp>
#include <boost/variant.hpp>

#include "Macro.h"

namespace parser {
namespace heidenhain {

enum class EBeginEnd : int8_t
{
    begin,
    end
};

LEFT_STREAM_OP_DECL(EBeginEnd)
RIGHT_STREAM_OP_DECL(EBeginEnd)

enum class EUnit : int8_t
{
    mm,
    inch
};

LEFT_STREAM_OP_DECL(EUnit)
RIGHT_STREAM_OP_DECL(EUnit)

struct ProgramBeginEndData
{
    EBeginEnd   begin_end;
    std::string name;
    EUnit       unit;
};

struct CommentData
{
    char        semicolon;
    std::string value;
    char        tilde;
};

struct LineNumberData
{
    uint32_t value;
};

struct CycleDef
{
    std::string                  word1; // CYCL
    std::string                  word2; // DEF or CALL
    boost::optional<std::string> value;
    boost::optional<char>        dot;
    boost::optional<std::string> value2;
};

struct CycleParam
{
    std::string                  name; // usually 'Q'
    uint32_t                     id;
    char                         assign; // '='
    boost::optional<char>        sign;
    std::string                  value;
    boost::optional<char>        dot;
    boost::optional<std::string> value2;
};

struct GeneralAttributeData
{
    std::string                  word;
    boost::optional<char>        sign;
    boost::optional<char>        name; // usually 'Q'
    boost::optional<std::string> value;
    boost::optional<char>        dot;
    boost::optional<std::string> value2;
    boost::optional<char>        tilde;
};

PARSER_API std::ostream& operator<<(std::ostream& os, const ProgramBeginEndData& data);
PARSER_API std::ostream& operator<<(std::ostream& os, const CommentData& data);
PARSER_API std::ostream& operator<<(std::ostream& os, const LineNumberData& data);
PARSER_API std::ostream& operator<<(std::ostream& os, const CycleDef& data);
PARSER_API std::ostream& operator<<(std::ostream& os, const CycleParam& data);
PARSER_API std::ostream& operator<<(std::ostream& os, const GeneralAttributeData& data);

using AttributeVariant =
    boost::variant<LineNumberData, CommentData, ProgramBeginEndData, GeneralAttributeData, CycleDef, CycleParam>;

} // namespace heidenhain
} // namespace parser
