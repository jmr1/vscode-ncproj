#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

struct unique_code_exception : std::runtime_error
{
    explicit unique_code_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

class UniqueCode
{
public:
    void operator()(const word_map& word_grammar, const std::vector<AttributeVariant>& value, ELanguage language);
};

} // namespace fanuc
} // namespace parser
