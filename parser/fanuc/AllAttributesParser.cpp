#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning(disable : 4348)
#endif

#include "AllAttributesParser.h"

#include <array>
#include <iomanip>
#include <sstream>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#ifndef M_E
#define M_E (2.7182818284590452354)
#endif

#ifndef NDEBUG
#define BOOST_SPIRIT_DEBUG
#endif

#include <boost/algorithm/string/trim.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "AxesRotator.h"
#include "CodeGroupsVerifier.h"
#include "FileSplitter.h"
#include "GrammarSelector.h"
#include "MacroEvaluatedCodeVerifier.h"
#include "MacroEvaluator.h"
#include "MessageTextImpl.h"
#include "RangeVerifier.h"
#include "RotationVerifier.h"
#include "SameWordInBlockVerifier.h"
#include "SystemMacroAssigner.h"
#include "UniqueCode.h"

#include "AllAttributesParserGrammar.h"

namespace parser {
namespace fanuc {

AllAttributesParser::AllAttributesParser(FanucWordGrammar&& grammar, std::vector<std::string>&& operations,
                                         code_groups_map&& gcodes, code_groups_map&& mcodes,
                                         ParserSettings&& parser_settings, OtherSettings&& other_settings,
                                         EFanucParserType fanuc_parser_type,
                                         bool             instantiateWithoutNCSettings /*= false*/)
    : word_grammar(std::move(grammar))
    , active_word_grammar(&word_grammar.metric)
    , allowed_operations(std::move(operations))
    , gcode_groups(std::move(gcodes))
    , mcode_groups(std::move(mcodes))
    , unit_converter(word_grammar)
    , renumberer(word_grammar)
    , parser_settings(std::move(parser_settings))
    , other_settings(std::move(other_settings))
    , fanuc_parser_type(fanuc_parser_type)
{
    build_symbols();

    if (instantiateWithoutNCSettings)
    {
        all_attr_grammar = std::make_unique<all_attributes_grammar<pos_iterator_type>>(
            *active_word_grammar, char_sym, decimal_sym, assignable_sym, other_settings.language);
    }
}

void AllAttributesParser::set_parser_settings(const ParserSettings& parser_settings)
{
    this->parser_settings = parser_settings;
}

EDriverUnits AllAttributesParser::get_unit_system() const
{
    return active_word_grammar == &word_grammar.metric ? EDriverUnits::Millimeter : EDriverUnits::Inch;
}

const macro_map& AllAttributesParser::get_macro_values() const
{
    return macro_values;
}

void AllAttributesParser::reset_macro_values(bool init /*= true*/)
{
    macro_values.clear();
    if (init)
        init_macro_values();
}

void AllAttributesParser::init_macro_values()
{
    macro_values.insert(std::make_pair(macro_map_key{3101, -1}, M_PI));
    macro_values.insert(std::make_pair(macro_map_key{3102, -1}, M_E));
}

void AllAttributesParser::reset_attributes_path_calculator()
{
    attr_path_calc->reset();
}

const PathResult& AllAttributesParser::get_path_result() const
{
    return attr_path_calc->get_path_result();
}

const TimeResult& AllAttributesParser::get_time_result() const
{
    return attr_path_calc->get_time_result();
}

const word_range_map& AllAttributesParser::get_word_range() const
{
    return attr_path_calc->get_word_range();
}

void AllAttributesParser::set_renumber_start_value(unsigned int value)
{
    renumberer.set_start_value(value);
}

void AllAttributesParser::renumber_reset()
{
    renumberer.reset();
}

void AllAttributesParser::set_ncsettings(EMachineTool machine_tool, EMachineToolType machine_tool_type,
                                         MachinePointsData&& machine_points_data, Kinematics&& kinematics,
                                         CncDefaultValues&& cnc_default_values, ZeroPoint&& zero_point)
{
    this->machine_tool      = machine_tool;
    this->machine_tool_type = machine_tool_type;
    this->zero_point        = std::move(zero_point);
    active_word_grammar     = cnc_default_values.default_driver_units == EDriverUnits::Millimeter ? &word_grammar.metric
                                                                                                  : &word_grammar.imperial;
    MachinePointsData machine_points_data_zero_point;

    const std::map<std::string, double> zero_point_axis_map = {
        {"X", zero_point.x}, {"Y", zero_point.y}, {"Z", zero_point.z}};

    for (const auto& it : machine_points_data.tool_exchange_point)
    {
        machine_points_data_zero_point.tool_exchange_point[it.first] =
            it.second - zero_point_axis_map.count(it.first) ? zero_point_axis_map.at(it.first) : 0;
    }

    for (const auto& it : machine_points_data.machine_base_point)
    {
        machine_points_data_zero_point.machine_base_point[it.first] =
            it.second - zero_point_axis_map.count(it.first) ? zero_point_axis_map.at(it.first) : 0;
    }

    attr_path_calc =
        std::make_unique<AttributesPathCalculator>(machine_tool, machine_tool_type, machine_points_data_zero_point,
                                                   kinematics, cnc_default_values, other_settings.language);

    all_attr_grammar = std::make_unique<all_attributes_grammar<pos_iterator_type>>(
        *active_word_grammar, char_sym, decimal_sym, assignable_sym, other_settings.language);
}

void AllAttributesParser::build_symbols()
{
    assignable_sym.clear();
    decimal_sym.clear();
    char_sym.clear();

    char_sym.add(",", ",");
    constexpr std::array<std::string_view, 3> skip_symbols = {"%", "O", "/"};
    for (const auto& x : *active_word_grammar)
    {
        if (x.second.word_type == WordType::assignable)
        {
            assignable_sym.add(x.first, x.first);
            continue;
        }

        if (std::find_if(std::cbegin(skip_symbols), std::cend(skip_symbols), [&](auto v) { return x.first == v; }) !=
            std::cend(skip_symbols))
            continue;
        else
            decimal_sym.add(x.first, x.first);
    }

    for (auto op : allowed_operations)
    {
        if (std::find_if(std::cbegin(skip_symbols), std::cend(skip_symbols), [&](auto v) { return op == v; }) !=
            std::cend(skip_symbols))
            continue;
        decimal_sym.add(op, op);
    }
}

bool AllAttributesParser::parse(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg, const ParserSettings& parser_settings)
{
    return parse(line, data, static_cast<FanucAttributeData&>(value).value, message, single_line_msg, parser_settings);
}

bool AllAttributesParser::parse(int line, std::string_view data, std::vector<AttributeVariant>& value,
                                std::string& message, bool single_line_msg, const ParserSettings& parser_settings)
{
    bool              ret{};
    bool              evaluated_code_part{};
    pos_iterator_type position_begin(data.cbegin()), iter = position_begin, position_end(data.cend());
    pos_iterator_type position_begin2;

    try
    {
        all_attr_grammar->clear_message();
        ret = qi::phrase_parse(iter, position_end, *all_attr_grammar, qi::blank, value);

        UniqueCode()(*active_word_grammar, value, other_settings.language);

        if (!parser_settings.evaluate_macro)
        {
            message = all_attr_grammar->get_message();
            return ret;
        }

        if (!MacroEvaluator::evaluate(macro_values, line, value, message, single_line_msg, other_settings.language))
        {
            message = all_attr_grammar->get_message();
            return false;
        }

        std::vector<AttributeVariant> value2;

        auto evaluated_code =
            MacroEvaluatedCodeVerifier()(allowed_operations, macro_values, line, value, other_settings.language);
        if (!evaluated_code.empty())
        {
            std::string_view evaluated_code_view = evaluated_code;

            position_begin2         = pos_iterator_type(evaluated_code_view.cbegin());
            pos_iterator_type iter2 = position_begin2, position_end2(evaluated_code_view.cend());
            evaluated_code_part     = true;
            ret                     = qi::phrase_parse(iter2, position_end2, *all_attr_grammar, qi::blank, value2);
            evaluated_code_part     = false;

            GrammarSelector()(word_grammar, active_word_grammar, value2);
        }

        SystemMacroAssigner()(macro_values, line, evaluated_code.empty() ? value : value2);

        if (parser_settings.ncsettings_code_analysis)
        {
            RangeVerifier()(machine_tool_type, attr_path_calc->get_kinematics(),
                            evaluated_code.empty() ? value : value2, other_settings.language);
            rotation_verifier(attr_path_calc->get_cnc_default_values(), evaluated_code.empty() ? value : value2,
                              other_settings.language);
        }

        if (parser_settings.verify_code_groups)
            CodeGroupsVerifier()(macro_values, gcode_groups, mcode_groups, line,
                                 evaluated_code.empty() ? value : value2, other_settings.language);

        if (fanuc_parser_type == EFanucParserType::HaasLathe || fanuc_parser_type == EFanucParserType::HaasMill)
            SameWordInBlockVerifier()("M", evaluated_code.empty() ? value : value2, other_settings.language);

        if (parser_settings.calculate_path)
        {
            if (attr_path_calc.get())
                attr_path_calc->evaluate(evaluated_code.empty() ? value : value2, get_unit_system(), macro_values,
                                         line);

            if (parser_settings.zero_point_analysis && attr_path_calc.get())
                RangeVerifier()(attr_path_calc->get_kinematics(), zero_point, attr_path_calc->get_word_range(),
                                other_settings.language);
        }

        GrammarSelector()(word_grammar, active_word_grammar, evaluated_code.empty() ? value : value2);
    }
    catch (const qi::expectation_failure<pos_iterator_type>& e)
    {
        size_t line   = get_line(e.first);
        size_t column = get_column(evaluated_code_part ? position_begin2 : position_begin, e.first);
        message       = all_attr_grammar->get_message();

        std::stringstream msg;
        msg << line << ":" << column << ": " << std::string(e.first, e.last);
        if (!message.empty())
            msg << " <- " << message;
        message = msg.str();
    }
    catch (const code_groups_exception& e)
    {
        message = e.what();
        return false;
    }
    catch (const unique_code_exception& e)
    {
        message = e.what();
        return false;
    }
    catch (const macro_evaluated_code_exception& e)
    {
        message = e.what();
        return false;
    }
    catch (const path_calc_exception& e)
    {
        message = e.what();
        return false;
    }
    catch (const range_verifier_exception& e)
    {
        message = e.what();
        return false;
    }
    catch (const same_word_verifier_exception& e)
    {
        message = e.what();
        return false;
    }
    catch (const rotation_verifier_exception& e)
    {
        message = e.what();
        return false;
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

bool AllAttributesParser::simple_parse(int line, std::string_view data, std::vector<AttributeVariant>& value,
                                       std::string& message, bool single_line_msg)
{
    bool              ret{};
    pos_iterator_type pos_begin(data.cbegin()), iter = pos_begin, pos_end(data.cend());

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
    return parse(line, data, static_cast<FanucAttributeData&>(value).value, message, single_line_msg);
}

bool AllAttributesParser::parse(int line, std::string_view data, std::vector<AttributeVariant>& value,
                                std::string& message, bool single_line_msg)
{
    return parse(line, data, value, message, single_line_msg, parser_settings);
}

bool AllAttributesParser::convert_length(int line, std::string_view data, AttributeVariantData& value,
                                         std::string& message, bool single_line_msg, UnitConversionType conversion_type)
{
    return convert_length(line, data, static_cast<FanucAttributeData&>(value).value, message, single_line_msg,
                          conversion_type);
}

bool AllAttributesParser::convert_length(int line, std::string_view data, std::vector<AttributeVariant>& value,
                                         std::string& message, bool single_line_msg, UnitConversionType conversion_type)
{
    if (!simple_parse(line, data, value, message, single_line_msg))
        return false;

    switch (conversion_type)
    {
    case UnitConversionType::metric_to_imperial:
        unit_converter.mm_to_inch(value);
        break;
    case UnitConversionType::imperial_to_metric:
        unit_converter.inch_to_mm(value);
        break;
    default:
        return false;
    }

    return true;
}

bool AllAttributesParser::remove_numbering(int line, const std::string& data, AttributeVariantData& value,
                                           std::string& message, bool single_line_msg)
{
    return remove_numbering(line, data, static_cast<FanucAttributeData&>(value).value, message, single_line_msg);
}

bool AllAttributesParser::remove_numbering(int line, const std::string& data, std::vector<AttributeVariant>& value,
                                           std::string& message, bool single_line_msg)
{
    auto ret = simple_parse(line, data, value, message, single_line_msg);

    renumberer.remove(value);

    return ret;
}

bool AllAttributesParser::renumber(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                                   bool single_line_msg, const RenumberSettings& renumber_settings)
{
    return renumber(line, data, static_cast<FanucAttributeData&>(value).value, message, single_line_msg,
                    renumber_settings);
}

bool AllAttributesParser::renumber(int line, const std::string& data, std::vector<AttributeVariant>& value,
                                   std::string& message, bool single_line_msg,
                                   const RenumberSettings& renumber_settings)
{
    auto ret = simple_parse(line, data, value, message, single_line_msg);

    renumberer.renumber(value, renumber_settings);

    return ret;
}

bool AllAttributesParser::rotate_axes(int line, std::string_view data, AttributeVariantData& value,
                                      std::string& message, bool single_line_msg,
                                      AxesRotatingOption axes_rotating_options)
{
    return rotate_axes(line, data, static_cast<FanucAttributeData&>(value).value, message, single_line_msg,
                       axes_rotating_options);
}

bool AllAttributesParser::rotate_axes(int line, std::string_view data, std::vector<AttributeVariant>& value,
                                      std::string& message, bool single_line_msg,
                                      AxesRotatingOption axes_rotating_options)
{
    auto ret = simple_parse(line, data, value, message, single_line_msg);

    try
    {
        AxesRotator()(axes_rotating_options, value);
    }
    catch (const std::out_of_range& e)
    {
        message = e.what();
        return false;
    }

    return ret;
}

bool AllAttributesParser::split_file(std::vector<std::string>&&                               data,
                                     std::vector<std::vector<std::vector<AttributeVariant>>>& value,
                                     std::string& message, bool single_line_msg, const FileSplitting& file_splitting)
{
    try
    {
        FileSplitter fs(file_splitting, other_settings.language);
        if (file_splitting.type == FileSplittingType::text)
        {
            auto tmp = fs.evaluate(std::move(data));

            int cnt{};
            value.resize(tmp.size());
            RenumberSettings rs{"N", 1, 1, 99999, false, false};
            for (const auto& t : tmp)
            {
                value[cnt].resize(t.size());
                for (size_t x = 0; x < t.size(); ++x)
                    renumber(static_cast<int>(x), t[x], value[cnt][x], message, single_line_msg, rs);
                ++cnt;
            }
        }
        else if (file_splitting.type == FileSplittingType::work_motion_path ||
                 file_splitting.type == FileSplittingType::work_motion_time)
        {
            std::vector<PathResult>                    vec_path_result;
            std::vector<TimeResult>                    vec_time_result;
            std::vector<std::vector<AttributeVariant>> vtmp;
            for (int x = 0; x < static_cast<int>(data.size()); ++x)
            {
                std::vector<AttributeVariant> v{};
                auto                          tmp(data[x]);
                boost::algorithm::trim(tmp);
                if (!tmp.empty() && !parse(x, data[x], v, message, single_line_msg, {true, false, true}))
                    return false;
                vtmp.emplace_back(std::move(v));
                if (file_splitting.type == FileSplittingType::work_motion_path)
                    vec_path_result.push_back(attr_path_calc->get_path_result());
                else
                    vec_time_result.push_back(attr_path_calc->get_time_result());
            }

            std::vector<AttributeVariant> retraction_plane;
            if (!file_splitting.retraction_plane.empty())
            {
                if (!simple_parse(0, file_splitting.retraction_plane, retraction_plane, message, single_line_msg))
                    return false;
            }

            if (file_splitting.type == FileSplittingType::work_motion_path)
                fs.evaluate(attr_path_calc->get_cnc_default_values(), std::move(vtmp), retraction_plane, value,
                            vec_path_result);
            else
                fs.evaluate(attr_path_calc->get_cnc_default_values(), std::move(vtmp), retraction_plane, value,
                            vec_time_result);

            RenumberSettings rs{"N", 1, 0, 0, false, false};
            for (auto& x : value)
                for (auto& y : x)
                    renumberer.renumber(y, rs);
        }
        else
        {
            std::vector<std::vector<AttributeVariant>> vtmp;
            for (int x = 0; x < static_cast<int>(data.size()); ++x)
            {
                std::vector<AttributeVariant> v{};
                auto                          tmp(data[x]);
                boost::algorithm::trim(tmp);
                if (!tmp.empty() && !simple_parse(x, data[x], v, message, single_line_msg))
                    return false;
                vtmp.emplace_back(std::move(v));
            }

            std::vector<AttributeVariant> retraction_plane;
            if (!file_splitting.retraction_plane.empty())
            {
                if (!simple_parse(0, file_splitting.retraction_plane, retraction_plane, message, single_line_msg))
                    return false;
            }

            fs.evaluate(attr_path_calc->get_cnc_default_values(), std::move(vtmp), retraction_plane, value);

            RenumberSettings rs{"N", 1, 0, 0, false, false};
            for (auto& x : value)
                for (auto& y : x)
                    renumberer.renumber(y, rs);
        }
    }
    catch (const std::out_of_range& e)
    {
        message = e.what();
        return false;
    }

    return true;
}

bool AllAttributesParser::remove_comment(int line, const std::string& data, AttributeVariantData& value,
                                         std::string& message, bool single_line_msg)
{
    return remove_comment(line, data, static_cast<FanucAttributeData&>(value).value, message, single_line_msg);
}

bool AllAttributesParser::remove_comment(int line, const std::string& data, std::vector<AttributeVariant>& value,
                                         std::string& message, bool single_line_msg)
{
    auto ret = simple_parse(line, data, value, message, single_line_msg);

    for (auto it = value.cbegin(); it != value.cend();)
    {
        if (const auto* p = boost::get<StringAttributeData>(&*it))
            if (p->word == "(")
                it = value.erase(it);
            else
                ++it;
        else
            ++it;
    }

    return ret;
}

bool AllAttributesParser::remove_optional_block(int line, const std::string& data, AttributeVariantData& value,
                                                std::string& message, bool single_line_msg)
{
    return remove_optional_block(line, data, static_cast<FanucAttributeData&>(value).value, message, single_line_msg);
}

bool AllAttributesParser::remove_optional_block(int line, const std::string& data, std::vector<AttributeVariant>& value,
                                                std::string& message, bool single_line_msg)
{
    auto ret = simple_parse(line, data, value, message, single_line_msg);

    // we are going to remove only those which begin the line
    // so:
    // /1 G1
    // /1 /2 G1
    // but not:
    // / G1
    // G1 /1 G2
    for (auto it = value.cbegin(); it != value.cend();)
    {
        if (const auto* p = boost::get<DecimalAttributeData>(&*it))
        {
            if (p->word == "/" && p->value)
            {
                it = value.erase(it);
                continue;
            }
        }
        break;
    }

    return ret;
}

} // namespace fanuc
} // namespace parser
