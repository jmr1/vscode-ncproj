#pragma once

#include <iomanip>
#include <sstream>

#include <string>

namespace parser {

template <typename T>
std::string to_string_trunc(const T& t)
{
    std::string str{std::to_string(t)};
    int         offset{1};
    if (str.find_last_not_of('0') == str.find('.'))
    {
        offset = 0;
    }
    str.erase(str.find_last_not_of('0') + offset, std::string::npos);
    return str;
}

template <typename T>
std::string to_string_leading_zeroes(const T& t, int num_zeroes)
{
    std::stringstream ss;
    ss << std::setw(num_zeroes) << std::setfill('0') << t;
    return ss.str();
}

} // namespace parser
