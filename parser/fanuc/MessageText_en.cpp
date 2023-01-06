#include "stdafx.h"

#include "MessageText.h"

namespace parser {
namespace fanuc {

const MessageText message_text_en[] = {
    {MessageName::ValueNotInRange, {"Value [", "] not in range [", ", ", "] for address [", "]"}},
    {MessageName::ValueMissingForHash, {"Value missing for parameter #"}},
    {MessageName::ValueNotInRangeForOptionalBlock, {"Value [", "] not in range [1,9] for character /"}},
    {MessageName::ForbiddenCharsInOptionalBlock, {"Forbidden characters in optional block value"}},
    {MessageName::ExpectedInteger, {"Expected integer number for address [", "]"}},
    {MessageName::ValueMissing, {"Value missing for address [", "]"}},
    {MessageName::DecimalSeparatorMissing, {"Missing decimal separator (dot) for address [", "]"}},
    {MessageName::ValueAfterDecimalSeparatorNotInRange,
     {"Value after dot [", "] not in range [", ", ", "] for address [", "]"}},
    {MessageName::ParseError, {"Parse error at line "}},
    {MessageName::TNotWithG, {"Address T can not appear in the block together with address G"}},
    {MessageName::MissingValue, {"Missing value for "}},
    {MessageName::MissingPreviousValue, {"Missing previous value for "}},
    {MessageName::MissingPreviousValueForEither, {"Missing previous values for either R or ", " and "}},
    {MessageName::ValueNotSet, {"Value ", " not set"}},
    {MessageName::ValueNotSetForEither, {"Not set values for either R or ", " and "}},
    {MessageName::ValueNotSetForEitherEither, {"Not set values for either R or ", " and ", " and "}},
    {MessageName::MissingInCodeGroups, {" is missing in Code Groups"}},
    {MessageName::InSameCodeGroup, {" and ", " are in the same Code Group = "}},
    {MessageName::CannotSplitFileNumberOfBlocks, {"Cannot split the file for number of blocks = 0"}},
    {MessageName::CannotSplitFileToolPathLength, {"Cannot split the file for tool path length = 0"}},
    {MessageName::CannotSplitFileToolTime, {"Cannot split the file for tool time = 0"}},
    {MessageName::WrongMacroDefinition, {"Wrong #", " variable definition"}},
    {MessageName::ValueHigherThanAllowed, {"Value ", " [", "] higher than max allowed value "}},
    {MessageName::ValueOutOfRange, {"Value ", " [", "] out of range [", ", ", "]"}},
    {MessageName::CodeNotUnique, {"Code ", " must be unique within a block"}},
    {MessageName::UnsupportedOperation, {"Unsupported operation: "}},
    {MessageName::UnsupportedBinaryOperation, {"Unsupported binary operation: [", "]"}},
    {MessageName::UnsupportedUnaryOperation, {"Unsupported unary operation: [", "]"}},
    {MessageName::UnsupportedFunction, {"Unsupported function: [", "]"}},
    {MessageName::DuplicateAddress, {"Same address [", "] in block not allowed"}},
    {MessageName::SNotAllowedWhenOperatorTurnsRotation, {"Address S not allowed when Operator turns on rotation"}},
    {MessageName::M3M4NotAllowedWhenOperatorTurnsRotation, {"M3/M4 not allowed when Operator turns on rotation"}},
    {MessageName::NonMatchingCloseBracket, {"Non matching close bracket"}},
};

const MessageText (&get_message_text_en())[static_cast<int>(MessageName::COUNT)]
{
    return message_text_en;
}

} // namespace fanuc
} // namespace parser
