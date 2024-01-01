#pragma once

#include <iomanip>

#ifndef NDEBUG
#define BOOST_SPIRIT_DEBUG
#endif

#include <boost/config/warning_disable.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/object.hpp> // construct
#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

#include "MessageTextImpl.h"
#include "get_spirit_parser.h"

#include "AllAttributesParserDefines.h"

namespace qi      = boost::spirit::qi;
namespace ascii   = boost::spirit::ascii;
namespace classic = boost::spirit::classic;
namespace phx     = boost::phoenix;
namespace fusion  = boost::fusion;

using word_symbols = qi::symbols<char, std::string>;

// clang-format off

// must be in global namespace
BOOST_FUSION_ADAPT_TPL_STRUCT(
    (T),
    (parser::fanuc::AttributeData) (T),
    (std::string, word)
    (T, value)
)

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (T)(U),
    (parser::fanuc::AttributeDataDecimal) (T)(U),
    (std::string, word)
    (boost::optional<char>, assign)
    (boost::optional<char>, sign)
    (boost::optional<std::string>, open_bracket)
    (boost::optional<std::string>, macro)
    (boost::optional<T>, value)
    (boost::optional<char>, dot)
    (boost::optional<U>, value2)
    (boost::optional<std::string>, close_bracket)
)

// clang-format on

namespace parser {
namespace fanuc {

struct status
{
    bool was_macro;
    bool was_macro_equal;
    int  open_bracket;
    int  close_bracket;
};

template <typename T>
std::ostream& operator<<(std::ostream& ostr, const AttributeData<T>& data)
{
    ostr << "AttributeData(" << data.word << "," << data.value << ")";
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const WordGrammar& grammar);

template <typename T>
bool verify_range(const word_map& word_grammar, const AttributeData<T>& data, std::string& message, ELanguage language)
{
    auto it = word_grammar.find(data.word);
    if (it == std::end(word_grammar))
        return true;
    auto& grammar = it->second;

    if (data.value < static_cast<T>(grammar.range_from) || data.value > static_cast<T>(grammar.range_to))
    {
        message += make_message(MessageName::ValueNotInRange, language, std::to_string(data.value),
                                std::to_string(grammar.range_from), std::to_string(grammar.range_to), grammar.word);
        return false;
    }

    return true;
}

template <>
bool verify_range(const word_map& word_grammar, const AttributeData<std::string>& data, std::string& message,
                  ELanguage language);

template <typename T, typename U>
bool verify_range(const word_map& word_grammar, const AttributeDataDecimal<T, U>& data, std::string& message,
                  ELanguage language, status& st)
{
    if (data.open_bracket)
        st.open_bracket += static_cast<int>((*data.open_bracket).size());

    auto it = word_grammar.find(data.word);
    if (it == std::end(word_grammar))
        return true;
    auto& grammar = it->second;

    if (data.word == "#")
        st.was_macro = true;
    else if (st.was_macro && data.word == "=")
        st.was_macro_equal = true;
    if (data.close_bracket)
        st.close_bracket += static_cast<int>((*data.close_bracket).size());

    if (data.close_bracket && st.open_bracket - st.close_bracket < 0)
    {
        message += make_message(MessageName::NonMatchingCloseBracket, language);
        return false;
    }

    if (data.macro)
    {
        if (!data.value)
        {
            message += make_message(MessageName::ValueMissingForHash, language);
            return false;
        }

        const AttributeData<T> tmp{*data.macro, *data.value};
        return verify_range(word_grammar, tmp, message, language);
    }

    if (data.word == "/")
    {
        if (!st.was_macro) // meaning this should be optional block
        {
            if (data.value)
            {
                unsigned int val = std::stoi(*data.value);
                if (val < 1 || val > 9)
                {
                    message +=
                        make_message(MessageName::ValueNotInRangeForOptionalBlock, language, std::to_string(val));
                    return false;
                }

                if (data.sign || data.dot || data.macro || data.value2)
                {
                    message += make_message(MessageName::ForbiddenCharsInOptionalBlock, language);
                    return false;
                }

                return true;
            }
            else
            {
                if (data.sign || data.dot || data.macro || data.value2)
                {
                    message += make_message(MessageName::ForbiddenCharsInOptionalBlock, language);
                    return false;
                }

                // no value is allowed and means 1
                return true;
            }
        }
    }

    if (data.value)
    {
        int val = std::stoi(*data.value);
        if (data.sign && *data.sign == '-')
            val *= -1;
        if (val < grammar.range_from || val > grammar.range_to)
        {
            message += make_message(MessageName::ValueNotInRange, language, std::to_string(val),
                                    std::to_string(grammar.range_from), std::to_string(grammar.range_to), grammar.word);
            return false;
        }

        if (data.word == "+" || data.word == "-" || data.word == "*" || data.word == "/")
        {
            if (st.open_bracket > 0)
            {
                if (data.close_bracket)
                {
                    st.open_bracket -= static_cast<int>((*data.close_bracket).size());
                    st.close_bracket -= static_cast<int>((*data.close_bracket).size());
                }
                return true;
            }
            if (!st.was_macro || !st.was_macro_equal)
                return false;
        }

        if (grammar.decimal_to == 0)
        {
            if (data.dot)
            {
                message += make_message(MessageName::ExpectedInteger, language, grammar.word);
                return false;
            }
            return true;
        }
    }
    else if (grammar.decimal_to == 0)
    {
        if (st.open_bracket || data.word == "=" || data.word == "+" || data.word == "-" || data.word == "*" ||
            data.word == "/")
            return true;
        message += make_message(MessageName::ValueMissing, language, grammar.word);
        return false;
    }

    if (grammar.decimal_to > 0 && !data.value2)
    {
        if (!data.value)
        {
            if (st.open_bracket || data.word == "=" || data.word == "+" || data.word == "-" || data.word == "*" ||
                data.word == "/")
                return true;
            message += make_message(MessageName::ValueMissing, language, grammar.word);
            return false;
        }
        return true;
    }

    if (data.dot && grammar.decimal_to == 0)
    {
        message += make_message(MessageName::ExpectedInteger, language, grammar.word);
        return false;
    }

    if (!data.dot || *data.dot != '.')
    {
        message += make_message(MessageName::DecimalSeparatorMissing, language, grammar.word);
        return false;
    }

    unsigned int value2 = std::stoi(*data.value2);

    if (value2 < grammar.decimal_from || value2 > grammar.decimal_to)
    {
        message += make_message(MessageName::ValueAfterDecimalSeparatorNotInRange, language, *data.value2,
                                std::to_string(grammar.decimal_from), std::to_string(grammar.decimal_to), grammar.word);
        return false;
    }

    return true;
}

template <typename Iterator>
class space_empty_attribute_grammar : public qi::grammar<Iterator, CharAttributeData()>
{
public:
    explicit space_empty_attribute_grammar(const word_symbols& symbols)
        : space_empty_attribute_grammar::base_type(space_empty_attribute)
    {
        space_empty_attribute = symbols >> qi::omit[+qi::blank] >> qi::attr(0);
        BOOST_SPIRIT_DEBUG_NODE(space_empty_attribute);
    }

private:
    qi::rule<Iterator, CharAttributeData()> space_empty_attribute;
};

template <typename Iterator>
class start_program_grammar : public qi::grammar<Iterator, CharAttributeData(), qi::blank_type>
{
public:
    start_program_grammar()
        : start_program_grammar::base_type(start_program)
    {
        start_program = qi::char_('%') >> qi::attr(0) >> qi::eoi;
        BOOST_SPIRIT_DEBUG_NODE(start_program);
    }

private:
    qi::rule<Iterator, CharAttributeData(), qi::blank_type> start_program;
};

template <typename Iterator>
class comment_attribute_grammar : public qi::grammar<Iterator, StringAttributeData()>
{
public:
    comment_attribute_grammar()
        : comment_attribute_grammar::base_type(comment)
    {
        comment = qi::char_('(') > *(qi::char_ - ')') > ')';
        BOOST_SPIRIT_DEBUG_NODE(comment);
    }

private:
    qi::rule<Iterator, StringAttributeData()> comment;
};

template <typename Iterator>
class program_name_grammar : public qi::grammar<Iterator, StringAttributeData()>
{
public:
    program_name_grammar()
        : program_name_grammar::base_type(program_name)
    {
        program_name = qi::char_('O') > +(qi::char_("a-zA-Z0-9_")) >> qi::omit[*qi::blank >> *qi::alpha];
        BOOST_SPIRIT_DEBUG_NODE(program_name);
    }

private:
    qi::rule<Iterator, StringAttributeData()> program_name;
};

template <typename Iterator, typename T>
class optional_block_grammar : public qi::grammar<Iterator, DecimalAttributeData(), qi::blank_type>
{
public:
    optional_block_grammar(const word_map& word_grammar, std::string& message, ELanguage language, status& st)
        : optional_block_grammar::base_type(start_optional_block)
    {
        digits      = +qi::char_("0-9"); // get_spirit_parser<DecimalAttributeData::type1>();
        digits_tens = +qi::char_("0-9"); // get_spirit_parser<DecimalAttributeData::type2>();
        auto verify_range_bind =
            phx::bind(verify_range<DecimalAttributeData::type1, DecimalAttributeData::type2>, phx::cref(word_grammar),
                      qi::_val, phx::ref(message), language, phx::ref(st));

        close_bracket = +qi::string("]");

        // Note: division including closing brackets is handled in either division_attribute_grammar or here
        optional_block = qi::char_("/") > &!qi::lit('/') > qi::attr(boost::none) > -qi::char_("-+") >
                         qi::attr(boost::none) > -qi::string("#") > qi::lexeme[-digits] > qi::no_skip[-qi::char_('.')] >
                         qi::no_skip[-digits_tens] > -close_bracket;
        start_optional_block %= optional_block[qi::_pass = verify_range_bind];
        BOOST_SPIRIT_DEBUG_NODES((digits)(digits_tens)(optional_block) /*(start_optional_block)*/);
    }

private:
    qi::rule<Iterator, DecimalAttributeData::type1()>          digits;
    qi::rule<Iterator, DecimalAttributeData::type2()>          digits_tens;
    qi::rule<Iterator, std::string()>                          close_bracket;
    qi::rule<Iterator, DecimalAttributeData(), qi::blank_type> optional_block;
    qi::rule<Iterator, DecimalAttributeData(), qi::blank_type> start_optional_block;
};

template <typename Iterator>
class division_attribute_grammar : public qi::grammar<Iterator, DecimalAttributeData(), qi::blank_type>
{
public:
    division_attribute_grammar(const word_map& word_grammar, std::string& message, ELanguage language, status& st)
        : division_attribute_grammar::base_type(start_division_attribute)
    {
        digits      = +qi::char_("0-9"); // get_spirit_parser<DecimalAttributeData::type1>();
        digits_tens = +qi::char_("0-9"); // get_spirit_parser<DecimalAttributeData::type2>();
        auto verify_range_bind =
            phx::bind(verify_range<DecimalAttributeData::type1, DecimalAttributeData::type2>, phx::cref(word_grammar),
                      qi::_val, phx::ref(message), language, phx::ref(st));

        open_bracket  = +qi::string("[");
        close_bracket = +qi::string("]");
        // Note: division including closing brackets is handled in either optional_block_grammar or here
        division_attribute = qi::char_("/") > &!qi::lit('/') > qi::attr(boost::none) > -qi::char_("-+") >
                             -open_bracket > -qi::string("#") > qi::lexeme[-digits] > qi::no_skip[-qi::char_('.')] >
                             qi::no_skip[-digits_tens] > -close_bracket;
        start_division_attribute %= division_attribute[qi::_pass = verify_range_bind];
        BOOST_SPIRIT_DEBUG_NODES(
            (digits)(digits_tens)(open_bracket)(close_bracket)(division_attribute) /*(start_division_attribute)*/);
    }

private:
    qi::rule<Iterator, DecimalAttributeData::type1()>          digits;
    qi::rule<Iterator, DecimalAttributeData::type2()>          digits_tens;
    qi::rule<Iterator, std::string()>                          open_bracket;
    qi::rule<Iterator, std::string()>                          close_bracket;
    qi::rule<Iterator, DecimalAttributeData(), qi::blank_type> division_attribute;
    qi::rule<Iterator, DecimalAttributeData(), qi::blank_type> start_division_attribute;
};

template <typename Iterator>
class assignable_attribute_grammar : public qi::grammar<Iterator, DecimalAttributeData(), qi::blank_type>
{
public:
    assignable_attribute_grammar(const word_map& word_grammar, const word_symbols& symbols, std::string& message,
                                 ELanguage language, status& st)
        : assignable_attribute_grammar::base_type(start_assignable_attribute)
    {
        digits      = +qi::char_("0-9"); // get_spirit_parser<DecimalAttributeData::type1>();
        digits_tens = +qi::char_("0-9"); // get_spirit_parser<DecimalAttributeData::type2>();
        auto verify_range_bind =
            phx::bind(verify_range<DecimalAttributeData::type1, DecimalAttributeData::type2>, phx::cref(word_grammar),
                      qi::_val, phx::ref(message), language, phx::ref(st));

        open_bracket         = +qi::string("[");
        close_bracket        = +qi::string("]");
        assignable_attribute = symbols > qi::char_("=") > -qi::char_("-+") > -open_bracket > -qi::string("#") >
                               qi::lexeme[-digits] > qi::no_skip[-qi::char_('.')] > qi::no_skip[-digits_tens] >
                               -close_bracket;
        start_assignable_attribute %= assignable_attribute[qi::_pass = verify_range_bind];
        BOOST_SPIRIT_DEBUG_NODES(
            (digits)(digits_tens)(open_bracket)(close_bracket)(assignable_attribute) /*(start_assignable_attribute)*/);
    }

private:
    qi::rule<Iterator, DecimalAttributeData::type1()>          digits;
    qi::rule<Iterator, DecimalAttributeData::type2()>          digits_tens;
    qi::rule<Iterator, std::string()>                          open_bracket;
    qi::rule<Iterator, std::string()>                          close_bracket;
    qi::rule<Iterator, DecimalAttributeData(), qi::blank_type> assignable_attribute;
    qi::rule<Iterator, DecimalAttributeData(), qi::blank_type> start_assignable_attribute;
};

template <typename Iterator>
class decimal_attribute_grammar : public qi::grammar<Iterator, DecimalAttributeData(), qi::blank_type>
{
public:
    decimal_attribute_grammar(const word_map& word_grammar, const word_symbols& symbols,
                              const word_symbols& assignable_symbols, std::string& message, ELanguage language,
                              status& st)
        : decimal_attribute_grammar::base_type(start_decimal_attribute)
    {
        digits      = +qi::char_("0-9"); // get_spirit_parser<DecimalAttributeData::type1>();
        digits_tens = +qi::char_("0-9"); // get_spirit_parser<DecimalAttributeData::type2>();
        auto verify_range_bind =
            phx::bind(verify_range<DecimalAttributeData::type1, DecimalAttributeData::type2>, phx::cref(word_grammar),
                      qi::_val, phx::ref(message), language, phx::ref(st));

        open_bracket  = +qi::string("[");
        close_bracket = +qi::string("]");
        // Note: division including closing brackets is handled in either optional_block_grammar or
        // division_attribute_grammar
        decimal_attribute = (&!(assignable_symbols >> qi::lit('=')) >> symbols) > qi::attr(boost::none) >
                            -qi::char_("-+") > -open_bracket > -qi::string("#") > qi::lexeme[-digits] >
                            qi::no_skip[-qi::char_('.')] > qi::no_skip[-digits_tens] > -close_bracket;
        start_decimal_attribute %= decimal_attribute[qi::_pass = verify_range_bind];
        BOOST_SPIRIT_DEBUG_NODES(
            (digits)(digits_tens)(open_bracket)(close_bracket)(decimal_attribute) /*(start_decimal_attribute)*/);
    }

private:
    qi::rule<Iterator, DecimalAttributeData::type1()>          digits;
    qi::rule<Iterator, DecimalAttributeData::type2()>          digits_tens;
    qi::rule<Iterator, std::string()>                          open_bracket;
    qi::rule<Iterator, std::string()>                          close_bracket;
    qi::rule<Iterator, DecimalAttributeData(), qi::blank_type> decimal_attribute;
    qi::rule<Iterator, DecimalAttributeData(), qi::blank_type> start_decimal_attribute;
};

template <typename Iterator>
class all_attributes_grammar : public qi::grammar<Iterator, std::vector<AttributeVariant>(), qi::blank_type>
{
public:
    all_attributes_grammar(const word_map& word_grammar, const word_symbols& char_sym, const word_symbols& decimal_sym,
                           const word_symbols& assignable_sym, std::string& message, ELanguage language)
        : all_attributes_grammar::base_type(line_attribute_vec)
        , char_eag_rule(char_sym)
        , optional_block_rule(word_grammar, message, language, st)
        , decimal_sag_rule(word_grammar, decimal_sym, assignable_sym, message, language, st)
        , division_sag_rule(word_grammar, message, language, st)
        , assignable_sag_rule(word_grammar, assignable_sym, message, language, st)
    {
        line_attribute     = (qi::no_skip[char_eag_rule] | decimal_sag_rule | division_sag_rule | assignable_sag_rule |
                          optional_block_rule | comment_rule | start_program_rule | program_name_rule);
        line_attribute_vec = +line_attribute > qi::eoi;
        BOOST_SPIRIT_DEBUG_NODES((line_attribute)(line_attribute_vec));
    }

private:
    space_empty_attribute_grammar<Iterator>                             char_eag_rule;
    optional_block_grammar<Iterator, int>                               optional_block_rule;
    decimal_attribute_grammar<Iterator>                                 decimal_sag_rule;
    division_attribute_grammar<Iterator>                                division_sag_rule;
    assignable_attribute_grammar<Iterator>                              assignable_sag_rule;
    comment_attribute_grammar<Iterator>                                 comment_rule;
    start_program_grammar<Iterator>                                     start_program_rule;
    program_name_grammar<Iterator>                                      program_name_rule;
    qi::rule<Iterator, AttributeVariant(), qi::blank_type>              line_attribute;
    qi::rule<Iterator, std::vector<AttributeVariant>(), qi::blank_type> line_attribute_vec;
    status                                                              st{};
};

using pos_iterator_type = boost::spirit::classic::position_iterator2<boost::spirit::istream_iterator>;
extern template class all_attributes_grammar<pos_iterator_type>;

} // namespace fanuc
} // namespace parser
