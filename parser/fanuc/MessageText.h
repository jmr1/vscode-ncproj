#pragma once

#include "parser_export.h"

#include <initializer_list>
#include <string_view>

#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

enum class MessageName
{
    ValueNotInRange,
    ValueMissingForHash,
    ValueNotInRangeForOptionalBlock,
    ForbiddenCharsInOptionalBlock,
    ExpectedInteger,
    ValueMissing,
    DecimalSeparatorMissing,
    ValueAfterDecimalSeparatorNotInRange,
    ParseError,
    TNotWithG,
    MissingValue,
    MissingPreviousValue,
    MissingPreviousValueForEither,
    ValueNotSet,
    ValueNotSetForEither,
    ValueNotSetForEitherEither,
    MissingInCodeGroups,
    InSameCodeGroup,
    CannotSplitFileNumberOfBlocks,
    CannotSplitFileToolPathLength,
    CannotSplitFileToolTime,
    WrongMacroDefinition,
    ValueHigherThanAllowed,
    ValueOutOfRange,
    CodeNotUnique,
    UnsupportedOperation,
    UnsupportedBinaryOperation,
    UnsupportedUnaryOperation,
    UnsupportedFunction,
    DuplicateAddress,

    COUNT // must be last
};

struct MessageText
{
    MessageName                             msg_name;
    std::initializer_list<std::string_view> msg_list;
};

PARSER_API const MessageText (&get_message_text_en())[static_cast<int>(MessageName::COUNT)];
PARSER_API const MessageText (&get_message_text_pl())[static_cast<int>(MessageName::COUNT)];

} // namespace fanuc
} // namespace parser
