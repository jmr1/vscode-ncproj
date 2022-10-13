#pragma once

#include "parser_export.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

struct same_word_verifier_exception : std::runtime_error
{
    explicit same_word_verifier_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

class PARSER_API SameWordInBlockVerifier
{
public:
    void operator()(const std::string& word, const std::vector<AttributeVariant>& value, ELanguage language);
};

} // namespace fanuc
} // namespace parser
