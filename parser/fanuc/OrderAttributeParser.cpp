#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning(disable : 4348)
#endif

#include "OrderAttributeParser.h"

#include <iomanip>

#ifndef NDEBUG
#define BOOST_SPIRIT_DEBUG
#endif

#include <boost/config/warning_disable.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

#include "get_spirit_parser.h"

namespace qi      = boost::spirit::qi;
namespace ascii   = boost::spirit::ascii;
namespace classic = boost::spirit::classic;

namespace parser {
namespace fanuc {

template <typename Iterator, typename T1, typename T2>
class order_attribute_grammar : public qi::grammar<Iterator, std::tuple<T1, T2>(), qi::blank_type>
{
public:
    order_attribute_grammar(char c1, char c2)
        : order_attribute_grammar::base_type(order_attribute_rule)
    {
        attr_rule1 = get_spirit_parser<T1>();
        BOOST_SPIRIT_DEBUG_NODE(attr_rule1);
        attr_rule2 = get_spirit_parser<T2>();
        BOOST_SPIRIT_DEBUG_NODE(attr_rule2);
        order_attribute_rule = qi::omit[*(ascii::graph - c1)] >> qi::lexeme[qi::lit(c1) > attr_rule1] >>
                               qi::omit[*(ascii::graph - c2)] >> qi::lexeme[qi::lit(c2) > attr_rule2] >>
                               qi::omit[*ascii::graph];
        BOOST_SPIRIT_DEBUG_NODE(order_attribute_rule);
    }

private:
    qi::rule<Iterator, T1()>                                 attr_rule1;
    qi::rule<Iterator, T2()>                                 attr_rule2;
    qi::rule<Iterator, std::tuple<T1, T2>(), qi::blank_type> order_attribute_rule;
};

template <typename T1, typename T2>
bool OrderAttributeParser::parse(const std::string& data, char c1, char c2, std::tuple<T1, T2>& value,
                                 std::string& message)
{
    std::istringstream input(data);

    // iterate over stream input
    typedef std::istreambuf_iterator<char> base_iterator_type;
    base_iterator_type                     in_begin(input);

    // convert input iterator to forward iterator, usable by spirit parser
    typedef boost::spirit::multi_pass<base_iterator_type> forward_iterator_type;
    forward_iterator_type                                 fwd_begin = boost::spirit::make_default_multi_pass(in_begin);
    forward_iterator_type                                 fwd_end;

    // wrap forward iterator with position iterator, to record the position
    typedef classic::position_iterator2<forward_iterator_type> pos_iterator_type;
    pos_iterator_type                                          position_begin(fwd_begin, fwd_end);
    pos_iterator_type                                          position_end;

    bool                                               ret = false;
    order_attribute_grammar<pos_iterator_type, T1, T2> order_attr_gr(c1, c2);

    try
    {
        ret = qi::phrase_parse(position_begin, position_end, order_attr_gr, qi::blank, value);
    }
    catch (const qi::expectation_failure<pos_iterator_type>& e)
    {
        const classic::file_position_base<std::string>& pos = e.first.get_position();
        std::stringstream                               msg;
        msg << "Parse error at line " << pos.line << " column " << pos.column << ":" << std::endl
            << "'" << e.first.get_currentline() << "'" << std::endl
            << std::setw(pos.column) << " "
            << "^- here" << std::endl;
        message = msg.str();
    }

    return ret;
}

template PARSER_API bool OrderAttributeParser::parse<float, float>(const std::string& data, char c1, char c2,
                                                                   std::tuple<float, float>& value,
                                                                   std::string&              message);

} // namespace fanuc
} // namespace parser
