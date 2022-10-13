#include "stdafx.h"

#include <string>

#include <gtest/gtest.h>

#include <fanuc/MessageTextImpl.h>

namespace fanuc_test {

class MessageTextTest : public ::testing::Test
{
protected:
    void verify(std::string&& msg, std::string&& msg_expected)
    {
        EXPECT_EQ(msg_expected, msg);
    }
};

TEST_F(MessageTextTest, messageText_PL)
{
    parser::ELanguage language = parser::ELanguage::Polish;

    using namespace parser::fanuc;

    verify(make_message(MessageName::ValueNotInRange, language, "-1000", "-500", "500", "X"),
           "Wartość [-1000] nie mieści się w zakresie [-500, 500] dla adresu [X]");
    verify(make_message(MessageName::ValueMissingForHash, language, "1"), "Brak wartości dla parametru #1");
    verify(make_message(MessageName::ValueNotInRangeForOptionalBlock, language, "15"),
           "Wartość [15] nie mieści się w zakresie [1,9] dla znaku /");
    verify(make_message(MessageName::ForbiddenCharsInOptionalBlock, language),
           "Niedozwolone znaki w opcjonalnej wartości bloku");
    verify(make_message(MessageName::ExpectedInteger, language, "G"), "Oczekiwana liczba całkowita dla adresu [G]");
    verify(make_message(MessageName::ValueMissing, language, "Y"), "Brak wartości dla adresu [Y]");
    verify(make_message(MessageName::DecimalSeparatorMissing, language, "Z"),
           "Brak separatora dziesiętnego (kropki) dla adresu [Z]");
    verify(make_message(MessageName::ValueAfterDecimalSeparatorNotInRange, language, "7777", "0", "999", "X"),
           "Wartość po kropce [7777] nie mieści się w zakresie [0, 999] dla adresu [X]");
    // verify(make_stream_message(MessageName::ParseError, {"Błąd parsowania w linii "} },
    verify(make_message(MessageName::TNotWithG, language), "Adres T nie może wystąpić w bloku razem z adresem G");
    verify(make_message(MessageName::MissingValue, language, "X"), "Brak wartości dla X");
    verify(make_message(MessageName::MissingPreviousValue, language, "Y"), "Brak poprzedniej wartości dla Y");
    verify(make_message(MessageName::MissingPreviousValueForEither, language, "X", "Y"),
           "Brak poprzedniej wartości dla R lub X i Y");
    verify(make_message(MessageName::ValueNotSet, language, "F"), "Wartość F nie ustawiona");
    verify(make_message(MessageName::ValueNotSetForEither, language, "X", "Y"),
           "Nie ustawione wartości dla R lub X i Y");
    verify(make_message(MessageName::ValueNotSetForEitherEither, language, "X", "Y", "Z"),
           "Nie ustawione wartości dla R lub X i Y i Z");
    verify(make_message2(MessageName::MissingInCodeGroups, language, "M888"), "M888 nie występuje w Grupie Kodów");
    verify(make_message2(MessageName::InSameCodeGroup, language, "M3", "M4", "5"),
           "M3 i M4 są w tej samej Grupie Kodów = 5");
    verify(make_message(MessageName::CannotSplitFileNumberOfBlocks, language),
           "Nie można podzielić pliku dla liczby bloków = 0");
    verify(make_message(MessageName::CannotSplitFileToolPathLength, language),
           "Nie można podzielić pliku dla drogi narzędzia = 0");
    verify(make_message(MessageName::CannotSplitFileToolTime, language),
           "Nie można podzielić pliku dla czasu narzędzia = 0");
    verify(make_message(MessageName::WrongMacroDefinition, language, "5"), "Błędna definicja zmiennej #5");
    verify(make_message(MessageName::ValueHigherThanAllowed, language, "S", "78", "50"),
           "Wartość S [78] wyższa niż maksymalna dozwolona wartość 50");
    verify(make_message(MessageName::ValueOutOfRange, language, "X", "-1000", "-500", "500"),
           "Wartość X [-1000] poza zakresem [-500, 500]");
    verify(make_message(MessageName::CodeNotUnique, language, "T"), "Kod T musi być unikalny w obrębie bloku");
    verify(make_message(MessageName::UnsupportedOperation, language, "nil"), "Nieobsługiwana operacja: nil");
    verify(make_message(MessageName::UnsupportedBinaryOperation, language, "op"),
           "Nieobsługiwana operacja dwuargumentowa: [op]");
    verify(make_message(MessageName::UnsupportedUnaryOperation, language, "op"),
           "Nieobsługiwana operacja jednoargumentowa: [op]");
    verify(make_message(MessageName::UnsupportedFunction, language, "func"), "Nieobsługiwana funkcja: [func]");
}

} // namespace fanuc_test
