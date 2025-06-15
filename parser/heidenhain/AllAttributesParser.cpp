#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning(disable : 4348)
#endif

#include "AllAttributesParser.h"

#include <sstream>
#include <string>
#include <string_view>

#ifndef NDEBUG
#define BOOST_SPIRIT_DEBUG
#endif

#include <boost/algorithm/string/trim.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "AllAttributesParserGrammar.h"

namespace parser {
namespace heidenhain {

AllAttributesParser::AllAttributesParser(ParserSettings&& parser_settings, OtherSettings&& other_settings,
                                         bool instantiateWithoutNCSettings /*= false*/)
    : parser_settings(std::move(parser_settings))
    , other_settings(other_settings)
{
    build_symbols();

    if (instantiateWithoutNCSettings)
    {
        all_attr_grammar = std::make_unique<all_attributes_grammar<pos_iterator_type>>(sym);
    }
}

void AllAttributesParser::set_parser_settings(const ParserSettings& parser_settings)
{
    this->parser_settings = parser_settings;
}

void AllAttributesParser::set_ncsettings(EMachineTool machine_tool, EMachineToolType machine_tool_type,
                                         MachinePointsData&& machine_points_data, Kinematics&& kinematics,
                                         CncDefaultValues&& cnc_default_values, ZeroPoint&& zero_point)
{
    this->zero_point = std::move(zero_point);
    // active_word_grammar = cnc_default_values.default_driver_units == EDriverUnits::Millimeter ? &word_grammar.metric
    // : &word_grammar.imperial;
    MachinePointsData machine_points_data_zero_point;
    if (const auto it = machine_points_data.tool_exchange_point.find("X");
        it != std::end(machine_points_data.tool_exchange_point))
        machine_points_data_zero_point.tool_exchange_point["X"] = it->second - zero_point.x;
    if (const auto it = machine_points_data.tool_exchange_point.find("Y");
        it != std::end(machine_points_data.tool_exchange_point))
        machine_points_data_zero_point.tool_exchange_point["Y"] = it->second - zero_point.y;
    if (const auto it = machine_points_data.tool_exchange_point.find("Z");
        it != std::end(machine_points_data.tool_exchange_point))
        machine_points_data_zero_point.tool_exchange_point["Z"] = it->second - zero_point.z;
    if (const auto it = machine_points_data.machine_base_point.find("X");
        it != std::end(machine_points_data.machine_base_point))
        machine_points_data_zero_point.machine_base_point["X"] = it->second - zero_point.x;
    if (const auto it = machine_points_data.machine_base_point.find("Y");
        it != std::end(machine_points_data.machine_base_point))
        machine_points_data_zero_point.machine_base_point["Y"] = it->second - zero_point.y;
    if (const auto it = machine_points_data.machine_base_point.find("Z");
        it != std::end(machine_points_data.machine_base_point))
        machine_points_data_zero_point.machine_base_point["Z"] = it->second - zero_point.z;
    // attr_path_calc = std::make_unique<AttributesPathCalculator>(machine_points_data_zero_point, kinematics,
    // cnc_default_values);

    all_attr_grammar = std::make_unique<all_attributes_grammar<pos_iterator_type>>(sym);
}

void AllAttributesParser::build_symbols()
{
    sym.clear();

    for (auto op : allowed_operations)
        sym.add(op, op);
}

bool AllAttributesParser::parse(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg, const ParserSettings& parser_settings)
{
    return parse(line, data, static_cast<HeidenhainAttributeData&>(value).value, message, single_line_msg,
                 parser_settings);
}

bool AllAttributesParser::parse(int line, std::string_view data, std::vector<AttributeVariant>& value,
                                std::string& message, bool single_line_msg, const ParserSettings& parser_settings)
{
    pos_iterator_type pos_begin(data.cbegin()), iter = pos_begin, pos_end(data.cend());
    bool              ret{};

    try
    {
        all_attr_grammar->clear_message();
        ret = qi::phrase_parse(iter, pos_end, *all_attr_grammar, qi::blank, value);
    }
    catch (const qi::expectation_failure<pos_iterator_type>& e)
    {
        size_t line   = get_line(e.first);
        size_t column = get_column(pos_begin, e.first);
        message       = all_attr_grammar->get_message();

        std::stringstream msg;
        msg << line << ":" << column << ": " << std::string(e.first, e.last);
        if (!message.empty())
            msg << " <- " << message;
        message = msg.str();
    }
    catch (const std::out_of_range& e)
    {
        message = e.what();
        return false;
    }

    return ret;
}

bool AllAttributesParser::parse(int line, std::string_view data, std::string& message, bool single_line_msg)
{
    std::vector<AttributeVariant> value;
    return parse(line, data, value, message, single_line_msg);
}

bool AllAttributesParser::simple_parse(int line, std::string_view data, AttributeVariantData& value,
                                       std::string& message, bool single_line_msg)
{
    return simple_parse(line, data, static_cast<HeidenhainAttributeData&>(value).value, message, single_line_msg);
}

bool AllAttributesParser::simple_parse(int line, std::string_view data, std::vector<AttributeVariant>& value,
                                       std::string& message, bool single_line_msg)
{
    pos_iterator_type pos_begin(data.cbegin()), iter = pos_begin, pos_end(data.cend());
    bool              ret{};

    try
    {
        all_attr_grammar->clear_message();
        ret = qi::phrase_parse(iter, pos_end, *all_attr_grammar, qi::blank, value);
    }
    catch (const qi::expectation_failure<pos_iterator_type>& e)
    {
        size_t line   = get_line(e.first);
        size_t column = get_column(pos_begin, e.first);
        message       = all_attr_grammar->get_message();

        std::stringstream msg;
        msg << line << ":" << column << ": " << std::string(e.first, e.last);
        if (!message.empty())
            msg << " <- " << message;
        message = msg.str();
    }
    catch (const std::out_of_range& e)
    {
        message = e.what();
        return false;
    }

    return ret;
}

bool AllAttributesParser::parse(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg)
{
    return parse(line, data, static_cast<HeidenhainAttributeData&>(value).value, message, single_line_msg);
}

bool AllAttributesParser::parse(int line, std::string_view data, std::vector<AttributeVariant>& value,
                                std::string& message, bool single_line_msg)
{
    return parse(line, data, value, message, single_line_msg, parser_settings);
}

} // namespace heidenhain
} // namespace parser
