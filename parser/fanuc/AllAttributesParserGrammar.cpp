#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning(disable : 4348)
#endif

#include "AllAttributesParserGrammar.h"

namespace parser {
namespace fanuc {

std::ostream& operator<<(std::ostream& ostr, const WordGrammar& grammar)
{
    ostr << "WordGrammar(" << grammar.word << ", " << grammar.range_from << ", " << grammar.range_to
         << grammar.decimal_from << ", " << grammar.decimal_to << ")";
    return ostr;
}

template <>
bool verify_range(const word_map& word_grammar, const AttributeData<std::string>& data, std::string& message,
                  ELanguage language)
{
    try
    {
        const AttributeData<int> tmp{data.word, std::stoi(data.value)};
        return verify_range(word_grammar, tmp, message, language);
    }
    catch (std::exception& e)
    {
        message += e.what();
        return false;
    }
}

template class all_attributes_grammar<pos_iterator_type>;

} // namespace fanuc
} // namespace parser
