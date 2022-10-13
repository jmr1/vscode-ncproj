#include "stdafx.h"

#include "SameWordInBlockVerifier.h"

#include "MessageTextImpl.h"
#include <boost/variant.hpp>

#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

class SameWordInBlockVerifierVisitor : public boost::static_visitor<>
{
public:
    SameWordInBlockVerifierVisitor(const std::string& word, ELanguage language)
        : word(word)
        , language(language)
    {
    }

    void operator()(const DecimalAttributeData& data)
    {
        if (data.word == word)
        {
            if (was_word)
                throw same_word_verifier_exception(make_message(MessageName::DuplicateAddress, language, word));
            was_word = true;
        }
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

private:
    const std::string& word;
    const ELanguage    language;
    bool               was_word{};
};

void SameWordInBlockVerifier::operator()(const std::string& word, const std::vector<AttributeVariant>& value, ELanguage language)
{
    SameWordInBlockVerifierVisitor same_word_verifier_visitor(word, language);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(same_word_verifier_visitor, value[x]);
}

} // namespace fanuc
} // namespace parser
