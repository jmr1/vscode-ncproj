#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning(disable : 4348)
#endif

#include "MessageTextImpl.h"
#include "ValueCalculator.h"

#include <cmath>
#include <iomanip>
#include <limits>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#ifndef NDEBUG
#define BOOST_SPIRIT_DEBUG
#endif

#include <boost/config/warning_disable.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/variant.hpp>

namespace qi = boost::spirit::qi;

namespace parser {
namespace fanuc {

struct eval_exception : std::runtime_error
{
    explicit eval_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

struct nil
{
};
struct signed_type;
struct function_type;
struct program;

typedef boost::variant<nil, double, boost::recursive_wrapper<signed_type>, boost::recursive_wrapper<function_type>,
                       boost::recursive_wrapper<program>>
    operand_type;

struct signed_type
{
    char         sign;
    operand_type operand;
};

struct function_type
{
    std::string  name;
    operand_type operand;
};

struct operation
{
    std::string  operator_name;
    operand_type operand;
};

struct program
{
    operand_type         first;
    std::list<operation> rest;
};

// print function for debugging
inline std::ostream& operator<<(std::ostream& out, nil)
{
    out << "nil";
    return out;
}

} // namespace fanuc
} // namespace parser

// clang-format off

// must be in global namespace
BOOST_FUSION_ADAPT_STRUCT(
    parser::fanuc::signed_type,
    (char, sign)
    (parser::fanuc::operand_type, operand)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::fanuc::function_type,
    (std::string, name)
    (parser::fanuc::operand_type, operand)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::fanuc::operation,
    (std::string, operator_name)
    (parser::fanuc::operand_type, operand)
)

BOOST_FUSION_ADAPT_STRUCT(
    parser::fanuc::program,
    (parser::fanuc::operand_type, first)
    (std::list<parser::fanuc::operation>, rest)
)

// clang-format on

namespace parser {
namespace fanuc {

struct printer
{
    void operator()(nil) const
    {
    }

    void operator()(double n) const
    {
        std::cout << n;
    }

    void operator()(operation const& x) const
    {
        boost::apply_visitor(*this, x.operand);
        if (x.operator_name == "+")
            std::cout << " add";
        else if (x.operator_name == "-")
            std::cout << " subt";
        else if (x.operator_name == "*")
            std::cout << " mult";
        else if (x.operator_name == "/")
            std::cout << " div";
        else
            std::cout << " " << x.operator_name;
    }

    void operator()(signed_type const& x) const
    {
        boost::apply_visitor(*this, x.operand);
        switch (x.sign)
        {
        case '-':
            std::cout << " neg";
            break;
        case '+':
            std::cout << " pos";
            break;
        }
    }

    void operator()(function_type const& x) const
    {
        boost::apply_visitor(*this, x.operand);
        std::cout << " " << x.name;
    }

    void operator()(program const& x) const
    {
        boost::apply_visitor(*this, x.first);
        for (operation const& oper : x.rest)
        {
            std::cout << ' ';
            (*this)(oper);
        }
    }
};

static uint64_t uint32_to_bcd(uint32_t usi)
{
    uint64_t shift  = 16;
    uint64_t result = (usi % 10);

    while ((usi = (usi / 10)))
    {
        result += (usi % 10) * shift;
        shift *= 16; // weirdly, it's not possible to left shift more than 32 bits
    }
    return result;
}

static uint32_t bcd_to_ui32(uint64_t bcd)
{
    uint64_t mask = 0x000f;
    uint64_t pwr  = 1;

    uint64_t i = (bcd & mask);
    while ((bcd = (bcd >> 4)))
    {
        pwr *= 10;
        i += (bcd & mask) * pwr;
    }
    return (uint32_t)i;
}

class eval
{
public:
    eval(const macro_map& macro_values, ELanguage language)
        : macro_val(macro_values)
        , language(language)
    {
    }

    double operator()(nil) const
    {
        // BOOST_ASSERT(0);
        // return 0;
        throw eval_exception(make_message(MessageName::UnsupportedOperation, language, "nil"));
    }

    double operator()(double n) const
    {
        return n;
    }

    double operator()(operation const& x, double lhs) const
    {
        double rhs = boost::apply_visitor(*this, x.operand);
        if (x.operator_name == "+")
            return lhs + rhs;
        else if (x.operator_name == "-")
            return lhs - rhs;
        else if (x.operator_name == "*")
            return lhs * rhs;
        else if (x.operator_name == "/")
            return lhs / rhs;
        else if (x.operator_name == "AND")
            return static_cast<int>(lhs) & static_cast<int>(rhs);
        else if (x.operator_name == "OR")
            return static_cast<int>(lhs) | static_cast<int>(rhs);
        else if (x.operator_name == "XOR")
            return static_cast<int>(lhs) ^ static_cast<int>(rhs);
        else if (x.operator_name == "MOD")
            return static_cast<int>(lhs) % static_cast<int>(rhs);

        // BOOST_ASSERT(0);
        // return 0;
        throw eval_exception(make_message(MessageName::UnsupportedBinaryOperation, language, x.operator_name));
    }

    double operator()(signed_type const& x) const
    {
        double rhs = boost::apply_visitor(*this, x.operand);
        switch (x.sign)
        {
        case '-':
            return -rhs;
        case '+':
            return +rhs;
        case '#': {
#ifdef max
#undef max
#endif
            const auto it =
                macro_val.lower_bound({static_cast<decltype(macro_map_key::id)>(rhs), std::numeric_limits<int>::max()});
            if (it != std::end(macro_val) && it->first.id == static_cast<decltype(macro_map_key::id)>(rhs))
                return it->second;
            throw eval_exception(
                make_message(MessageName::ValueMissing, language, std::to_string(static_cast<unsigned int>(rhs))));
            // return 0;
        }
        }

        // BOOST_ASSERT(0);
        // return 0;
        throw eval_exception(make_message(MessageName::UnsupportedUnaryOperation, language, x.sign));
    }

    double operator()(function_type const& x) const
    {
        double rhs = boost::apply_visitor(*this, x.operand);
        if (x.name == "abs")
            return std::abs(rhs);
        else if (x.name == "acos")
            return std::acos(rhs * M_PI / 180);
        else if (x.name == "asin")
            return std::asin(rhs * M_PI / 180);
        else if (x.name == "atan")
            return std::atan(rhs * M_PI / 180);
        else if (x.name == "ceil")
            return std::ceil(rhs);
        else if (x.name == "cos")
            return std::cos(rhs * M_PI / 180);
        else if (x.name == "cosh")
            return std::cosh(rhs * M_PI / 180);
        else if (x.name == "exp")
            return std::exp(rhs);
        else if (x.name == "floor")
            return std::floor(rhs);
        else if (x.name == "log10")
            return std::log10(rhs);
        else if (x.name == "log")
            return std::log(rhs);
        else if (x.name == "sin")
            return std::sin(rhs * M_PI / 180);
        else if (x.name == "sqrt")
            return std::sqrt(rhs);
        else if (x.name == "tan")
            return std::tan(rhs * M_PI / 180);
        else if (x.name == "tanh")
            return std::tanh(rhs * M_PI / 180);
        else if (x.name == "round")
            return std::round(rhs);
        else if (x.name == "bcd")
            return bcd_to_ui32(static_cast<uint64_t>(rhs));
        else if (x.name == "bin")
            return static_cast<double>(uint32_to_bcd(static_cast<uint32_t>(rhs)));

        // BOOST_ASSERT(0);
        // return 0;
        throw eval_exception(make_message(MessageName::UnsupportedFunction, language, x.name));
    }

    double operator()(program const& x) const
    {
        double state = boost::apply_visitor(*this, x.first);
        for (operation const& oper : x.rest)
        {
            state = (*this)(oper, state);
        }
        return state;
    }

private:
    const macro_map& macro_val;
    const ELanguage  language;
};

template <typename Iterator>
class calculator : public qi::grammar<Iterator, program(), qi::blank_type>
{
public:
    explicit calculator(const function_symbols& sym)
        : calculator::base_type(expression)
    {
        function_call = sym > qi::omit[qi::char_("([")] > expression > qi::omit[qi::char_(")]")];

        expression = term >> *((qi::char_('+') > term) | (qi::char_('-') > term) | (qi::string("OR") > term) |
                               (qi::string("XOR") > term));

        term = factor >> *((qi::char_('*') > factor) | (qi::char_('/') > factor) | (qi::string("AND") > factor) |
                           (qi::string("MOD") > factor));

        factor = qi::double_ | (qi::omit[qi::char_("([")] > expression > qi::omit[qi::char_(")]")]) |
                 (qi::char_('-') > factor) | (qi::char_('+') > factor) | (qi::char_('#') > factor) | function_call;

        BOOST_SPIRIT_DEBUG_NODES((expression)(term)(function_call)(factor));
    }

private:
    qi::rule<Iterator, program(), qi::blank_type>       expression;
    qi::rule<Iterator, program(), qi::blank_type>       term;
    qi::rule<Iterator, function_type(), qi::blank_type> function_call;
    qi::rule<Iterator, operand_type(), qi::blank_type>  factor;
};

ValueCalculator::ValueCalculator()
{
    build_symbols();
}

void ValueCalculator::build_symbols()
{
    sym.add("ABS", "abs");
    sym.add("ACOS", "acos");
    sym.add("ASIN", "asin");
    sym.add("ATAN", "atan");
    sym.add("FUP", "ceil");
    sym.add("COS", "cos");
    sym.add("COSH", "cosh");
    sym.add("EXP", "exp");
    sym.add("FIX", "floor");
    sym.add("LOG10", "log10");
    sym.add("LOG", "log");
    sym.add("SIN", "sin");
    sym.add("SINH", "sinh");
    sym.add("SQRT", "sqrt");
    sym.add("TAN", "tan");
    sym.add("TANH", "tanh");
    sym.add("ROUND", "round");
    sym.add("BCD", "bcd");
    sym.add("BIN", "bin");
}

bool ValueCalculator::parse(const std::string& data, std::string& message, bool single_line_msg,
                            const macro_map& macro_values, double& value, ELanguage language)
{
    using pos_iterator_type = boost::spirit::line_pos_iterator<std::string::const_iterator>;

    std::istringstream input(data);
    pos_iterator_type  pos_begin(data.cbegin()), iter = pos_begin, pos_end(data.cend());

    bool ret{};

    calculator<pos_iterator_type> calc(sym);

    try
    {
        program p;

        ret = qi::phrase_parse(iter, pos_end, calc, qi::blank, p);

#ifndef NDEBUG
        printer()(p);
#endif
        value = eval(macro_values, language)(p);
    }
    catch (const qi::expectation_failure<pos_iterator_type>& e)
    {
        size_t line   = get_line(e.first);
        size_t column = get_column(pos_begin, e.first);

        std::stringstream msg;
        msg << line << ":" << column << ": " << std::string(e.first, e.last);
        message = msg.str();
    }
    catch (const eval_exception& e)
    {
        message = e.what();
        return false;
    }

    return ret;
}

} // namespace fanuc
} // namespace parser
