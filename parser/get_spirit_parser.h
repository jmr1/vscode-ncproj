#pragma once

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

namespace {

template <typename T>
auto get_spirit_parser();

template <>
[[maybe_unused]] auto get_spirit_parser<double>()
{
    return qi::double_;
}
template <>
[[maybe_unused]] auto get_spirit_parser<float>()
{
    return qi::float_;
}
template <>
[[maybe_unused]] auto get_spirit_parser<char>()
{
    return qi::char_;
}
template <>
[[maybe_unused]] auto get_spirit_parser<int>()
{
    return qi::int_;
}
template <>
[[maybe_unused]] auto get_spirit_parser<unsigned int>()
{
    return qi::uint_;
}

} // namespace
