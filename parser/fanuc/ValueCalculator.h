#pragma once

#include "parser_export.h"

#include <map>
#include <string>

#include <boost/spirit/include/qi_symbols.hpp>

#include "MacroDefines.h"
#include "GeneralParserDefines.h"

namespace qi = boost::spirit::qi;

namespace parser {
namespace fanuc {

using function_symbols = qi::symbols<char, std::string>;

class PARSER_API ValueCalculator
{
public:
    ValueCalculator();

    bool parse(const std::string& data, std::string& message, bool single_line_msg, const macro_map& macro_values,
               double& value, ELanguage language);

private:
    void build_symbols();

private:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    function_symbols sym;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
};

} // namespace fanuc
} // namespace parser
