#include "stdafx.h"

#include "UniqueCode.h"

#include <set>
#include <string>

#include <boost/variant.hpp>

#include "MessageTextImpl.h"

namespace parser {
namespace fanuc {

class UniqueCodeVisitor : public boost::static_visitor<>
{
public:
    UniqueCodeVisitor(const word_map& word_grammar, ELanguage language)
        : word_grammar(word_grammar)
        , language(language)
    {
    }

    void operator()(const DecimalAttributeData& data)
    {
        auto it_word = word_grammar.find(data.word);
        if (it_word == std::end(word_grammar))
            return;
        if (!it_word->second.unique)
            return;
        if (codes.find(data.word) == std::end(codes))
        {
            codes.insert(data.word);
            return;
        }
        throw unique_code_exception(make_message(MessageName::CodeNotUnique, language, data.word));
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

private:
    const word_map&       word_grammar;
    const ELanguage       language;
    std::set<std::string> codes;
};

void UniqueCode::operator()(const word_map& word_grammar, const std::vector<AttributeVariant>& value, ELanguage language)
{
    UniqueCodeVisitor unique_code_visitor(word_grammar, language);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(unique_code_visitor, value[x]);
}

} // namespace fanuc
} // namespace parser
