#pragma once

#include <iomanip>

#ifndef NDEBUG
#define BOOST_SPIRIT_DEBUG
#endif

#include <boost/config/warning_disable.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp> // construct
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

#include "get_spirit_parser.h"
//#include "MessageTextImpl.h"

#include "AllAttributesParserDefines.h"

namespace qi      = boost::spirit::qi;
namespace ascii   = boost::spirit::ascii;
namespace classic = boost::spirit::classic;
namespace phx     = boost::phoenix;
namespace fusion  = boost::fusion;

using word_symbols = qi::symbols<char, std::string>;

// clang-format off

// must be in global namespace

BOOST_FUSION_ADAPT_STRUCT(
    parser::heidenhain::CommentData,
    (char, semicolon)
    (std::string, value)
    (char, tilde)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::heidenhain::ProgramBeginEndData,
    (parser::heidenhain::EBeginEnd, begin_end)
    (std::string, name)
    (parser::heidenhain::EUnit, unit)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::heidenhain::LineNumberData,
    (uint32_t, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::heidenhain::CycleDef,
    (std::string, word1)
    (std::string, word2)
    (boost::optional<std::string>, value)
    (boost::optional<char>, dot)
    (boost::optional<std::string>, value2)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::heidenhain::CycleParam,
    (std::string, name)
    (uint32_t, id)
    (char, assign)
    (boost::optional<char>, sign)
    (std::string, value)
    (boost::optional<char>, dot)
    (boost::optional<std::string>, value2)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::heidenhain::GeneralAttributeData,
    (std::string, word)
    (boost::optional<char>, sign)
    (boost::optional<char>, name)
    (boost::optional<std::string>, value)
    (boost::optional<char>, dot)
    (boost::optional<std::string>, value2)
    (boost::optional<char>, tilde)
)

// clang-format on

namespace parser {
namespace heidenhain {

template <typename Iterator>
class start_program_grammar : public qi::grammar<Iterator, ProgramBeginEndData()>
{
public:
    start_program_grammar()
        : start_program_grammar::base_type(start_program)
    {
        begin_end.add("BEGIN", EBeginEnd::begin)("END", EBeginEnd::end);

        unit.add("MM", EUnit::mm)("INCH", EUnit::inch);

        start_program = begin_end > qi::omit[+qi::blank] > qi::lit("PGM") > qi::omit[+qi::blank] >
                        *(qi::char_("a-zA-Z0-9_")) > qi::omit[+qi::blank] > unit > qi::omit[+qi::space | qi::eoi] >
                        !qi::char_("a-zA-Z0-9_");
        BOOST_SPIRIT_DEBUG_NODE((start_program));
    }

private:
    qi::symbols<char, heidenhain::EBeginEnd>  begin_end;
    qi::symbols<char, heidenhain::EUnit>      unit;
    qi::rule<Iterator, ProgramBeginEndData()> start_program;
};

template <typename Iterator>
class general_attribute_grammar : public qi::grammar<Iterator, GeneralAttributeData()>
{
public:
    general_attribute_grammar(const word_symbols& symbols, bool& was_cycle_def)
        : general_attribute_grammar::base_type(general_attribute)
    {
        attr_value               = +qi::char_("0-9");
        attr_value2              = +qi::char_("0-9");
        attr_value_quoted_string = qi::char_('"') > +qi::char_("a-zA-Z0-9") > qi::char_('"');
        general_attribute        = (symbols | +(qi::char_("a-zA-Z.:/")) | qi::char_("#") | qi::char_("=") |
                             attr_value_quoted_string) > qi::omit[*qi::blank] > -qi::char_("-+") > -qi::char_("Q") >
                            -attr_value > qi::no_skip[-qi::char_('.')] > qi::no_skip[-attr_value2] > -qi::char_("~");
        BOOST_SPIRIT_DEBUG_NODES((attr_value_quoted_string)(general_attribute));
    }

private:
    qi::rule<Iterator, std::string()>          attr_value;
    qi::rule<Iterator, std::string()>          attr_value2;
    qi::rule<Iterator, std::string()>          attr_value_quoted_string;
    qi::rule<Iterator, GeneralAttributeData()> general_attribute;
};

template <typename Iterator>
class comment_attribute_grammar : public qi::grammar<Iterator, CommentData()>
{
public:
    comment_attribute_grammar()
        : comment_attribute_grammar::base_type(comment)
    {
        comment = qi::char_(';') > *(qi::char_ - '~') > (qi::char_('~') | qi::attr(0)) >> qi::eoi;
        BOOST_SPIRIT_DEBUG_NODE(comment);
    }

private:
    qi::rule<Iterator, CommentData()> comment;
};

template <typename Iterator>
class line_number_attribute_grammar : public qi::grammar<Iterator, LineNumberData()>
{
public:
    line_number_attribute_grammar()
        : line_number_attribute_grammar::base_type(line_number)
    {
        line_number = qi::uint_ > qi::blank;
        BOOST_SPIRIT_DEBUG_NODE(line_number);
    }

private:
    qi::rule<Iterator, LineNumberData()> line_number;
};

template <typename Iterator>
class cycle_def_attribute_grammar : public qi::grammar<Iterator, CycleDef()>
{
public:
    cycle_def_attribute_grammar(bool& was_cycle_def)
        : cycle_def_attribute_grammar::base_type(cycle_def)
    {
        attr_value  = +qi::char_("0-9");
        attr_value2 = +qi::char_("0-9");
        cycle_def   = qi::string("CYCL") > qi::omit[+qi::blank] > (qi::string("DEF") | qi::string("CALL")) >
                    qi::omit[*qi::blank] > -attr_value > qi::no_skip[-qi::char_('.')] > qi::no_skip[-(attr_value2)] >>
                    qi::eps(phx::ref(was_cycle_def) = true);
        BOOST_SPIRIT_DEBUG_NODE(cycle_def);
    }

private:
    qi::rule<Iterator, std::string()> attr_value;
    qi::rule<Iterator, std::string()> attr_value2;
    qi::rule<Iterator, CycleDef()>    cycle_def;
};

template <typename Iterator>
class cycle_param_attribute_grammar : public qi::grammar<Iterator, CycleParam()>
{
public:
    cycle_param_attribute_grammar()
        : cycle_param_attribute_grammar::base_type(cycle_param)
    {
        attr_value  = +qi::char_("0-9");
        attr_value2 = +qi::char_("0-9");
        cycle_param = qi::char_("Q") > qi::int_ > qi::omit[*qi::blank] > qi::char_("=") > qi::omit[*qi::blank] >
                      -qi::char_("-+") > -(attr_value) > qi::no_skip[-qi::char_('.')] > qi::no_skip[-(attr_value2)];
        BOOST_SPIRIT_DEBUG_NODE(cycle_param);
    }

private:
    qi::rule<Iterator, std::string()> attr_value;
    qi::rule<Iterator, std::string()> attr_value2;
    qi::rule<Iterator, CycleParam()>  cycle_param;
};

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif
template <typename Iterator>
class all_attributes_grammar : public qi::grammar<Iterator, std::vector<AttributeVariant>(), qi::blank_type>
{
public:
    all_attributes_grammar(const word_symbols& sym, std::string& message)
        : all_attributes_grammar::base_type(line_attribute_vec)
        , cycle_def_rule(was_cycle_def)
        , general_attribute_rule(sym, was_cycle_def)
    {
        line_attribute =
            (cycle_def_rule | cycle_param_rule | comment_rule | start_program_rule | general_attribute_rule);
        line_attribute_vec = -line_number_rule > +line_attribute > qi::eoi;
        BOOST_SPIRIT_DEBUG_NODES((line_attribute)(line_attribute_vec));
    }

private:
    comment_attribute_grammar<Iterator>                                 comment_rule;
    line_number_attribute_grammar<Iterator>                             line_number_rule;
    cycle_def_attribute_grammar<Iterator>                               cycle_def_rule;
    cycle_param_attribute_grammar<Iterator>                             cycle_param_rule;
    start_program_grammar<Iterator>                                     start_program_rule;
    general_attribute_grammar<Iterator>                                 general_attribute_rule;
    qi::rule<Iterator, AttributeVariant(), qi::blank_type>              line_attribute;
    qi::rule<Iterator, std::vector<AttributeVariant>(), qi::blank_type> line_attribute_vec;
    bool                                                                was_cycle_def{};
};
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

using pos_iterator_type = boost::spirit::classic::position_iterator2<boost::spirit::istream_iterator>;
extern template class all_attributes_grammar<pos_iterator_type>;

} // namespace heidenhain
} // namespace parser
