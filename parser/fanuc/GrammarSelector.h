#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"

namespace parser {
namespace fanuc {

class GrammarSelector
{
public:
    void operator()(const FanucWordGrammar& fanuc_word_grammar, const word_map*& active_word_grammar,
                    const std::vector<AttributeVariant>& value);
};

} // namespace fanuc
} // namespace parser
