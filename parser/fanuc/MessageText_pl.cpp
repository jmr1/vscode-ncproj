#include "stdafx.h"

#include "MessageText.h"

namespace {
char const* operator"" _C(const char8_t* str, std::size_t)
{
    return reinterpret_cast<const char*>(str);
}
}


namespace parser {
namespace fanuc {

const MessageText message_text_pl[] = {
    {MessageName::ValueNotInRange, {u8"Wartość ["_C, u8"] nie mieści się w zakresie ["_C, ", ", u8"] dla adresu ["_C, "]"}},
    {MessageName::ValueMissingForHash, {u8"Brak wartości dla parametru #"_C}},
    {MessageName::ValueNotInRangeForOptionalBlock, {u8"Wartość ["_C, u8"] nie mieści się w zakresie [1,9] dla znaku /"_C}},
    {MessageName::ForbiddenCharsInOptionalBlock, {u8"Niedozwolone znaki w opcjonalnej wartości bloku"_C}},
    {MessageName::ExpectedInteger, {u8"Oczekiwana liczba całkowita dla adresu ["_C, "]"}},
    {MessageName::ValueMissing, {u8"Brak wartości dla adresu ["_C, "]"}},
    {MessageName::DecimalSeparatorMissing, {u8"Brak separatora dziesiętnego (kropki) dla adresu ["_C, "]"}},
    {MessageName::ValueAfterDecimalSeparatorNotInRange,
     {u8"Wartość po kropce ["_C, u8"] nie mieści się w zakresie ["_C, ", ", u8"] dla adresu ["_C, "]"}},
    {MessageName::ParseError, {u8"Błąd parsowania w linii "_C}},
    {MessageName::TNotWithG, {u8"Adres T nie może wystąpić w bloku razem z adresem G"_C}},
    {MessageName::MissingValue, {u8"Brak wartości dla "_C}},
    {MessageName::MissingPreviousValue, {u8"Brak poprzedniej wartości dla "_C}},
    {MessageName::MissingPreviousValueForEither, {u8"Brak poprzedniej wartości dla R lub "_C, " i "}},
    {MessageName::ValueNotSet, {u8"Wartość "_C, " nie ustawiona"}},
    {MessageName::ValueNotSetForEither, {u8"Nie ustawione wartości dla R lub "_C, " i "}},
    {MessageName::ValueNotSetForEitherEither, {u8"Nie ustawione wartości dla R lub "_C, " i ", " i "}},
    {MessageName::MissingInCodeGroups, {u8" nie występuje w Grupie Kodów"_C}},
    {MessageName::InSameCodeGroup, {u8" i "_C, u8" są w tej samej Grupie Kodów = "_C}},
    {MessageName::CannotSplitFileNumberOfBlocks, {u8"Nie można podzielić pliku dla liczby bloków = 0"_C}},
    {MessageName::CannotSplitFileToolPathLength, {u8"Nie można podzielić pliku dla drogi narzędzia = 0"_C}},
    {MessageName::CannotSplitFileToolTime, {u8"Nie można podzielić pliku dla czasu narzędzia = 0"_C}},
    {MessageName::WrongMacroDefinition, {u8"Błędna definicja zmiennej #"_C}},
    {MessageName::ValueHigherThanAllowed, {u8"Wartość "_C, " [", u8"] wyższa niż maksymalna dozwolona wartość "_C}},
    {MessageName::ValueOutOfRange, {u8"Wartość "_C, " [", u8"] poza zakresem ["_C, ", ", "]"}},
    {MessageName::CodeNotUnique, {u8"Kod "_C, u8" musi być unikalny w obrębie bloku"_C}},
    {MessageName::UnsupportedOperation, {u8"Nieobsługiwana operacja: "_C}},
    {MessageName::UnsupportedBinaryOperation, {u8"Nieobsługiwana operacja dwuargumentowa: ["_C, "]"}},
    {MessageName::UnsupportedUnaryOperation, {u8"Nieobsługiwana operacja jednoargumentowa: ["_C, "]"}},
    {MessageName::UnsupportedFunction, {u8"Nieobsługiwana funkcja: ["_C, "]"}},
    {MessageName::DuplicateAddress, {u8"Kolejny adres ["_C, "] niedozwolony w bloku"}},
    {MessageName::SNotAllowedWhenOperatorTurnsRotation, {u8"Niedozwolony adres S gdy obroty włącza Operator"_C}},
    {MessageName::M3M4NotAllowedWhenOperatorTurnsRotation, {u8"Niedozwolony M3/M4 gdy obroty włącza Operator"_C}},
    {MessageName::NonMatchingCloseBracket, {u8"Niedopasowany nawias zamykający"_C}},
};

const MessageText (&get_message_text_pl())[static_cast<int>(MessageName::COUNT)]
{
    return message_text_pl;
}

} // namespace fanuc
} // namespace parser
