#include "stdafx.h"

#include "MessageText.h"

namespace parser {
namespace fanuc {

const MessageText message_text_pl[] = {
    {MessageName::ValueNotInRange, {u8"Wartość [", u8"] nie mieści się w zakresie [", ", ", u8"] dla adresu [", "]"}},
    {MessageName::ValueMissingForHash, {u8"Brak wartości dla parametru #"}},
    {MessageName::ValueNotInRangeForOptionalBlock, {u8"Wartość [", u8"] nie mieści się w zakresie [1,9] dla znaku /"}},
    {MessageName::ForbiddenCharsInOptionalBlock, {u8"Niedozwolone znaki w opcjonalnej wartości bloku"}},
    {MessageName::ExpectedInteger, {u8"Oczekiwana liczba całkowita dla adresu [", "]"}},
    {MessageName::ValueMissing, {u8"Brak wartości dla adresu [", "]"}},
    {MessageName::DecimalSeparatorMissing, {u8"Brak separatora dziesiętnego (kropki) dla adresu [", "]"}},
    {MessageName::ValueAfterDecimalSeparatorNotInRange,
     {u8"Wartość po kropce [", u8"] nie mieści się w zakresie [", ", ", u8"] dla adresu [", "]"}},
    {MessageName::ParseError, {u8"Błąd parsowania w linii "}},
    {MessageName::TNotWithG, {u8"Adres T nie może wystąpić w bloku razem z adresem G"}},
    {MessageName::MissingValue, {u8"Brak wartości dla "}},
    {MessageName::MissingPreviousValue, {u8"Brak poprzedniej wartości dla "}},
    {MessageName::MissingPreviousValueForEither, {u8"Brak poprzedniej wartości dla R lub ", " i "}},
    {MessageName::ValueNotSet, {u8"Wartość ", " nie ustawiona"}},
    {MessageName::ValueNotSetForEither, {u8"Nie ustawione wartości dla R lub ", " i "}},
    {MessageName::ValueNotSetForEitherEither, {u8"Nie ustawione wartości dla R lub ", " i ", " i "}},
    {MessageName::MissingInCodeGroups, {u8" nie występuje w Grupie Kodów"}},
    {MessageName::InSameCodeGroup, {u8" i ", u8" są w tej samej Grupie Kodów = "}},
    {MessageName::CannotSplitFileNumberOfBlocks, {u8"Nie można podzielić pliku dla liczby bloków = 0"}},
    {MessageName::CannotSplitFileToolPathLength, {u8"Nie można podzielić pliku dla drogi narzędzia = 0"}},
    {MessageName::CannotSplitFileToolTime, {u8"Nie można podzielić pliku dla czasu narzędzia = 0"}},
    {MessageName::WrongMacroDefinition, {u8"Błędna definicja zmiennej #"}},
    {MessageName::ValueHigherThanAllowed, {u8"Wartość ", " [", u8"] wyższa niż maksymalna dozwolona wartość "}},
    {MessageName::ValueOutOfRange, {u8"Wartość ", " [", u8"] poza zakresem [", ", ", "]"}},
    {MessageName::CodeNotUnique, {u8"Kod ", u8" musi być unikalny w obrębie bloku"}},
    {MessageName::UnsupportedOperation, {u8"Nieobsługiwana operacja: "}},
    {MessageName::UnsupportedBinaryOperation, {u8"Nieobsługiwana operacja dwuargumentowa: [", "]"}},
    {MessageName::UnsupportedUnaryOperation, {u8"Nieobsługiwana operacja jednoargumentowa: [", "]"}},
    {MessageName::UnsupportedFunction, {u8"Nieobsługiwana funkcja: [", "]"}},
    {MessageName::DuplicateAddress, {u8"Kolejny adres [", "] niedozwolony w bloku"}},
};

const MessageText (&get_message_text_pl())[static_cast<int>(MessageName::COUNT)]
{
    return message_text_pl;
}

} // namespace fanuc
} // namespace parser
