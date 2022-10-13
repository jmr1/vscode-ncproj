#pragma once

#include <set>
#include <string>
#include <unordered_map>

#include <boost/functional/hash.hpp>

namespace parser {
namespace fanuc {

struct CodeGroupValue
{
    unsigned int code;
    unsigned int rest;

    bool operator==(const CodeGroupValue& data) const
    {
        return code == data.code && rest == data.rest;
    }
};

struct CodeGroupValueHasher
{
    size_t operator()(const CodeGroupValue& data) const
    {
        size_t seed = 0;

        boost::hash_combine(seed, boost::hash_value(data.code));
        boost::hash_combine(seed, boost::hash_value(data.rest));

        return seed;
    }
};

using code_groups_map = std::unordered_map<CodeGroupValue, std::set<std::string>, CodeGroupValueHasher>;

} // namespace fanuc
} // namespace parser
