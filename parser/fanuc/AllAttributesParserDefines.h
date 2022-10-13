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
namespace fanuc {

template <typename T>
struct AttributeData
{
    std::string word;
    T           value;

    using type = T;

    bool operator==(const AttributeData<T>& data) const
    {
        return word == data.word && value == data.value;
    }
};

// Supports decimal and integer types or macro types like G-#10, T#10
template <typename T, typename U>
struct AttributeDataDecimal
{
    std::string                  word;
    boost::optional<char>        assign; // '=' when assignable type
    boost::optional<char>        sign;
    boost::optional<std::string> open_bracket;
    boost::optional<std::string> macro; // here should be always sitting # character
    boost::optional<T>           value;
    boost::optional<char>        dot;
    boost::optional<U>           value2;
    boost::optional<std::string> close_bracket;

    using type1 = T;
    using type2 = U;
};

using StringAttributeData  = AttributeData<std::string>;
using CharAttributeData    = AttributeData<char>;
using DecimalAttributeData = AttributeDataDecimal<std::string, std::string>;

PARSER_API std::ostream& operator<<(std::ostream& os, const StringAttributeData& data);
PARSER_API std::ostream& operator<<(std::ostream& os, const CharAttributeData& data);
PARSER_API std::ostream& operator<<(std::ostream& os, const DecimalAttributeData& data);

using AttributeVariant = boost::variant<DecimalAttributeData, StringAttributeData, CharAttributeData>;

enum class WordType
{
    misc = 0,
    angle,
    calc,
    code,
    feed,
    length,
    macro,
    numbering,
    assignable
};

LEFT_STREAM_OP_DECL(WordType)
RIGHT_STREAM_OP_DECL(WordType)

struct WordGrammar
{
    std::string  word;
    int          range_from;
    int          range_to;
    unsigned int decimal_from;
    unsigned int decimal_to;
    bool         unique;
    WordType     word_type;
};

using word_map = std::unordered_map<std::string, WordGrammar>;

struct FanucWordGrammar
{
    word_map metric;
    word_map imperial;
};

// min, current, max
using word_range_map = std::map<std::string, std::tuple<double, double, double>>;

} // namespace fanuc
} // namespace parser
