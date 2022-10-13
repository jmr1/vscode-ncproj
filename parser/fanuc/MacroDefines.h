#pragma once

#include "parser_export.h"

#include <map>

namespace parser {
namespace fanuc {

struct macro_map_key
{
    unsigned int id;
    int          line;
};

struct macro_map_key_cmp
{
    bool operator()(const macro_map_key& lhs, const macro_map_key& rhs) const
    {
        if (lhs.id < rhs.id)
            return false;
        if (lhs.id == rhs.id && lhs.line <= rhs.line)
            return false;
        return true;
    }
};

using macro_map = std::map<macro_map_key, double, macro_map_key_cmp>;

} // namespace fanuc
} // namespace parser
