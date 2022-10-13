#pragma once

#include "parser_export.h"

#include <string>
#include <unordered_map>
#include <vector>

#include "AllAttributesParserDefines.h"

namespace parser {

struct RenumberSettings;

namespace fanuc {

class PARSER_API Renumberer
{
public:
    explicit Renumberer(const FanucWordGrammar& word_grammar);

    void remove(std::vector<AttributeVariant>& value);
    void renumber(std::vector<AttributeVariant>& value, const RenumberSettings& renumber_settings);
    void set_start_value(unsigned int value)
    {
        counter = value;
    }
    void reset()
    {
        counter           = 1;
        current_frequency = 0;
    }

private:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    FanucWordGrammar                                    word_grammar;
    std::unordered_map<std::string, const WordGrammar*> conv_word_grammar;
    std::vector<std::string>                            conv_words;
    unsigned int                                        counter{1};
    unsigned int                                        current_frequency{};
#ifdef _MSC_VER
#pragma warning(pop)
#endif
};

} // namespace fanuc
} // namespace parser
