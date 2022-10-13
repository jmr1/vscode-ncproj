#pragma once

#include "parser_export.h"

#include <string>
#include <tuple>

namespace parser {
namespace fanuc {

class PARSER_API OrderAttributeParser
{
public:
    template <typename T1, typename T2>
    bool parse(const std::string& data, char c1, char c2, std::tuple<T1, T2>& value, std::string& message);
};

} // namespace fanuc
} // namespace parser
