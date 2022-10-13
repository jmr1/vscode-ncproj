#include "stdafx.h"

#include "GrammarSelector.h"

#include <set>
#include <string>

#include <boost/variant.hpp>

namespace parser {
namespace fanuc {

class GrammarSelectorVisitor : public boost::static_visitor<>
{
public:
    GrammarSelectorVisitor(const FanucWordGrammar& fanuc_word_grammar, const word_map*& active_word_grammar)
        : fanuc_word_grammar(fanuc_word_grammar)
        , active_word_grammar(active_word_grammar)
    {
    }

    void operator()(const DecimalAttributeData& data)
    {
        if (data.word != "G")
            return;

        if (!data.value)
            return;

        unsigned int val = std::stoi(*data.value);
        if (val == 20)
            active_word_grammar = &fanuc_word_grammar.imperial;
        else if (val == 21)
            active_word_grammar = &fanuc_word_grammar.metric;
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

private:
    const FanucWordGrammar& fanuc_word_grammar;
    const word_map*&        active_word_grammar;
};

void GrammarSelector::operator()(const FanucWordGrammar& fanuc_word_grammar, const word_map*& active_word_grammar,
                                 const std::vector<AttributeVariant>& value)
{
    GrammarSelectorVisitor grammar_selector_visitor(fanuc_word_grammar, active_word_grammar);
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(grammar_selector_visitor, value[x]);
}

} // namespace fanuc
} // namespace parser
