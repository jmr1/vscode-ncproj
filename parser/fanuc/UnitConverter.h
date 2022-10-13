#pragma once

#include "parser_export.h"

#include <string>
#include <unordered_map>
#include <vector>

#include "AllAttributesParserDefines.h"

namespace parser {
namespace fanuc {

// Due to a bug in Ubuntu 18.04 gcc 7.4.0 when static linking in unordered_map cannot keep WordGrammar*
using ConvWordGrammar = std::unordered_map<std::string, WordGrammar>;

class PARSER_API UnitConverter
{
public:
    explicit UnitConverter(const FanucWordGrammar& word_grammar);

    void mm_to_inch(std::vector<AttributeVariant>& value);
    void inch_to_mm(std::vector<AttributeVariant>& value);

private:
    void cache_grammar(const word_map& wm, ConvWordGrammar& conv_word_grammar, std::vector<std::string>& conv_words);

private:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    FanucWordGrammar         word_grammar;
    ConvWordGrammar          conv_word_grammar_metric;
    ConvWordGrammar          conv_word_grammar_imperial;
    std::vector<std::string> conv_words_metric;
    std::vector<std::string> conv_words_imperial;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
};

} // namespace fanuc
} // namespace parser
