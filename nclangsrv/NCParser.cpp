#include "stdafx.h"

#include "NCParser.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <fanuc/AllAttributesParser.h>
#include <heidenhain/AllAttributesParser.h>

namespace pt = boost::property_tree;
namespace fs = std::filesystem;

using namespace parser;

static bool read_json(const std::string& grammar_path, const std::string& unit, fanuc::word_map& word_grammar_map,
                      std::vector<std::string>& operations)
{
    try
    {
        pt::ptree root;
        pt::read_json(grammar_path, root);

        for (const auto& u : root.get_child(unit))
        {
            word_grammar_map[u.first] = {u.second.get<decltype(fanuc::WordGrammar::word)>("word"),
                                         u.second.get<decltype(fanuc::WordGrammar::range_from)>("range_from"),
                                         u.second.get<decltype(fanuc::WordGrammar::range_to)>("range_to"),
                                         u.second.get<decltype(fanuc::WordGrammar::decimal_from)>("decimal_from"),
                                         u.second.get<decltype(fanuc::WordGrammar::decimal_to)>("decimal_to"),
                                         u.second.get<decltype(fanuc::WordGrammar::unique)>("unique"),
                                         u.second.get<decltype(fanuc::WordGrammar::word_type)>("type")};
        }

        for (const auto& o : root.get_child("operations"))
            operations.push_back(o.second.get_value<std::string>());
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

static bool read_json(const std::string& code_groups_path, fanuc::code_groups_map& code_groups)
{
    try
    {
        pt::ptree root;
        pt::read_json(code_groups_path, root);

        for (const auto& u : root.get_child("groups"))
        {
            for (const auto& group : u.second)
            {
                code_groups[{group.second.get<decltype(fanuc::CodeGroupValue::code)>("code"),
                             group.second.get<decltype(fanuc::CodeGroupValue::rest)>("rest")}]
                    .insert(u.first);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

static auto read_json_ncsettings(const std::string& path)
{
    ZeroPoint         zero_point{};
    MachinePointsData machine_points_data{};
    Kinematics        kinematics{};
    CncDefaultValues  cnc_default_values{};

    try
    {
        pt::ptree root;
        pt::read_json(path, root);

        {
            const auto& mpd = root.get_child("machine_points_data");
            for (const auto& pt : mpd.get_child("tool_exchange_point"))
                machine_points_data.tool_exchange_point.emplace(std::make_pair(
                    pt.first, pt.second.get_value<decltype(MachinePointsData::tool_exchange_point)::mapped_type>()));
            for (const auto& pt : mpd.get_child("machine_base_point"))
                machine_points_data.machine_base_point.emplace(std::make_pair(
                    pt.first, pt.second.get_value<decltype(MachinePointsData::machine_base_point)::mapped_type>()));
        }

        {
            const auto& k_pt = root.get_child("kinematics");
            for (const auto& pt : k_pt.get_child("cartesian_system_axis"))
            {
                kinematics.cartesian_system_axis.emplace(std::make_pair<const std::string&, AxisParameters>(
                    pt.first, {pt.second.get<decltype(AxisParameters::range_min)>("range_min"),
                               pt.second.get<decltype(AxisParameters::range_max)>("range_max")}));
            }
            auto& k                 = kinematics;
            k.max_working_feed      = k_pt.get<decltype(k.max_working_feed)>("max_working_feed");
            k.max_fast_feed         = k_pt.get<decltype(k.max_fast_feed)>("max_fast_feed");
            k.maximum_spindle_speed = k_pt.get<decltype(k.maximum_spindle_speed)>("maximum_spindle_speed");
            k.numer_of_items_in_the_warehouse =
                k_pt.get<decltype(k.numer_of_items_in_the_warehouse)>("numer_of_items_in_the_warehouse");
            k.tool_exchange_time      = k_pt.get<decltype(k.tool_exchange_time)>("tool_exchange_time");
            k.pallet_exchange_time    = k_pt.get<decltype(k.pallet_exchange_time)>("pallet_exchange_time");
            k.tool_measurement_time   = k_pt.get<decltype(k.tool_measurement_time)>("tool_measurement_time");
            k.diameter_programming_2x = k_pt.get<decltype(k.diameter_programming_2x)>("diameter_programming_2x");
            k.auto_measure_after_tool_selection =
                k_pt.get<decltype(k.auto_measure_after_tool_selection)>("auto_measure_after_tool_selection");
            k.pallet_exchange_code = k_pt.get<decltype(k.pallet_exchange_code)>("pallet_exchange_code");
            k.pallet_exchange_code_value =
                k_pt.get<decltype(k.pallet_exchange_code_value)>("pallet_exchange_code_value");
            k.tool_measurement_code = k_pt.get<decltype(k.tool_measurement_code)>("tool_measurement_code");
            k.tool_measurement_code_value =
                k_pt.get<decltype(k.tool_measurement_code_value)>("tool_measurement_code_value");
        }

        {
            const auto& cnc        = root.get_child("cnc_default_values");
            auto&       c          = cnc_default_values;
            c.default_motion       = cnc.get<decltype(c.default_motion)>("default_motion");
            c.default_work_plane   = cnc.get<decltype(c.default_work_plane)>("default_work_plane");
            c.default_driver_units = cnc.get<decltype(c.default_driver_units)>("default_driver_units");
            c.default_programming  = cnc.get<decltype(c.default_programming)>("default_programming");
            c.default_feed_mode    = cnc.get<decltype(c.default_feed_mode)>("default_feed_mode");
            c.default_rotation_direction =
                cnc.get<decltype(c.default_rotation_direction)>("default_rotation_direction");
            c.drill_cycle_z_value      = cnc.get<decltype(c.drill_cycle_z_value)>("drill_cycle_z_value");
            c.drill_cycle_return_value = cnc.get<decltype(c.drill_cycle_return_value)>("drill_cycle_return_value");
            c.default_rotation         = cnc.get<decltype(c.default_rotation)>("default_rotation");
            c.rapid_traverse_cancel_cycle =
                cnc.get<decltype(c.rapid_traverse_cancel_cycle)>("rapid_traverse_cancel_cycle");
            c.linear_interpolation_cancel_cycle =
                cnc.get<decltype(c.linear_interpolation_cancel_cycle)>("linear_interpolation_cancel_cycle");
            c.circular_interpolation_cw_cancel_cycle =
                cnc.get<decltype(c.circular_interpolation_cw_cancel_cycle)>("circular_interpolation_cw_cancel_cycle");
            c.circular_interpolation_ccw_cancel_cycle =
                cnc.get<decltype(c.circular_interpolation_ccw_cancel_cycle)>("circular_interpolation_ccw_cancel_cycle");
            c.cycle_cancel_starts_rapid_traverse =
                cnc.get<decltype(c.cycle_cancel_starts_rapid_traverse)>("cycle_cancel_starts_rapid_traverse");
            c.operator_turns_on_rotation =
                cnc.get<decltype(c.operator_turns_on_rotation)>("operator_turns_on_rotation");
            c.tool_number_executes_exchange =
                cnc.get<decltype(c.tool_number_executes_exchange)>("tool_number_executes_exchange");
            c.auto_rapid_traverse_after_tool_exchange =
                cnc.get<decltype(c.auto_rapid_traverse_after_tool_exchange)>("auto_rapid_traverse_after_tool_exchange");
        }

        {
            const auto& zp = root.get_child("zero_point");
            zero_point.x   = zp.get<decltype(ZeroPoint::x)>("X");
            zero_point.y   = zp.get<decltype(ZeroPoint::y)>("Y");
            zero_point.z   = zp.get<decltype(ZeroPoint::z)>("Z");
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return std::make_tuple(false, std::move(machine_points_data), std::move(kinematics),
                               std::move(cnc_default_values), std::move(zero_point));
        ;
    }

    return std::make_tuple(true, std::move(machine_points_data), std::move(kinematics), std::move(cnc_default_values),
                           std::move(zero_point));
}

namespace parser {
namespace fanuc {

class AttributesVisitor : public boost::static_visitor<>
{
public:
    explicit AttributesVisitor(std::string& text)
        : text(text)
    {
    }

    void operator()(const DecimalAttributeData& data) const
    {
        text += data.word;
        if (data.assign)
            text += *data.assign;
        if (data.sign)
            text += *data.sign;
        if (data.open_bracket)
            text += *data.open_bracket;
        if (data.macro)
            text += *data.macro;
        if (data.value)
            text += *data.value;
        if (data.dot)
            text += *data.dot;
        if (data.value2)
            text += *data.value2;
        if (data.close_bracket)
            text += *data.close_bracket;
    }

    void operator()(const StringAttributeData& data) const
    {
        text += data.word;
        text += data.value;
        if (data.word == "(")
            text += ")";
    }

    void operator()(const CharAttributeData& data) const
    {
        text += data.word;
        if (data.value != 0)
            text += data.value;
    }

private:
    std::string& text;
};

static void fill_parsed_values(const std::vector<AttributeVariant>& v, std::string& text)
{
    for (size_t x = 0; x < v.size(); ++x)
    {
        boost::apply_visitor(AttributesVisitor(text), v[x]);
        text += " ";
    }
}

} // namespace fanuc

namespace heidenhain {

class AttributesVisitor : public boost::static_visitor<>
{
public:
    explicit AttributesVisitor(std::string& text)
        : text(text)
    {
    }

private:
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif
    [[maybe_unused]] std::string& text;
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
};

static void fill_parsed_values(const std::vector<AttributeVariant>& v, std::string& text)
{
    /*for(size_t x = 0; x < v.size(); ++x)
    {
        boost::apply_visitor(AttributesVisitor(text), v[x]);
        text += " ";
    }*/
}

} // namespace heidenhain

} // namespace parser

NCParser::NCParser(const std::string& rootPath /*= ""*/)
    : cnc_type(ECncType::Fanuc)
    , machine_tool_type(EMachineToolType::Mill)
    , grammar_path("./conf/fanuc_mill/grammar.json")
    , gcode_groups_path("./conf/fanuc_mill/gcode_groups.json")
    , mcode_groups_path("./conf/fanuc_mill/mcode_groups.json")
    , grammar_unit(EDriverUnits::Millimeter)
    , unit_conversion_type(UnitConversionType::metric_to_imperial)
    , axes_rotating_option(AxesRotatingOption::Xrotate90degrees)
    , single_line_output(true)
    , convert_length(false)
    , calculate_path_time(false)
    , rotate(false)
{
    if (not rootPath.empty())
    {
        const auto fsRootPath = fs::path(rootPath);

        grammar_path      = fs::canonical(fsRootPath / fs::path(grammar_path)).string();
        gcode_groups_path = fs::canonical(fsRootPath / fs::path(gcode_groups_path)).string();
        mcode_groups_path = fs::canonical(fsRootPath / fs::path(mcode_groups_path)).string();
    }
}

void NCParser::set_cnc_type(ECncType value)
{
    cnc_type = value;
    redo_paths();
}

void NCParser::set_machine_tool_type(EMachineToolType value)
{
    machine_tool_type = value;
    redo_paths();
}

void NCParser::redo_paths()
{
    std::string new_path = "./conf/";

    switch (cnc_type)
    {
    case ECncType::Fanuc:
        switch (machine_tool_type)
        {
        case EMachineToolType::Mill:
            new_path += "fanuc_mill/";
            break;
        case EMachineToolType::Lathe:
        case EMachineToolType::Millturn:
            new_path += "fanuc_lathe/";
            break;
        }
        break;

    case ECncType::Haas:
        switch (machine_tool_type)
        {
        case EMachineToolType::Mill:
            new_path += "haas_mill/";
            break;
        case EMachineToolType::Lathe:
        case EMachineToolType::Millturn:
            new_path += "haas_lathe/";
            break;
        }
        break;

    case ECncType::Makino:
        switch (machine_tool_type)
        {
        case EMachineToolType::Mill:
            new_path += "makino_mill/";
            break;
        case EMachineToolType::Lathe:
        case EMachineToolType::Millturn:
            break;
        }
        break;

    case ECncType::Generic:
        switch (machine_tool_type)
        {
        case EMachineToolType::Mill:
            new_path += "generic_mill/";
            break;
        case EMachineToolType::Lathe:
        case EMachineToolType::Millturn:
            new_path += "generic_lathe/";
            break;
        }
        break;

    case ECncType::Heidenhain:
        break;
    }

    grammar_path      = new_path + "grammar.json";
    gcode_groups_path = new_path + "gcode_groups.json";
    mcode_groups_path = new_path + "mcode_groups.json";
}

std::vector<std::string> NCParser::parse(const std::string& code)
{
    fanuc::FanucWordGrammar  word_grammar;
    std::vector<std::string> operations;
    fanuc::code_groups_map   gcode_groups;
    fanuc::code_groups_map   mcode_groups;

    if (!read_json(grammar_path, "metric", word_grammar.metric, operations))
        return {"ERROR: Couldn't read metric from grammar"};
    if (!read_json(grammar_path, "imperial", word_grammar.imperial, operations))
        return {"ERROR: Couldn't read imperial from grammar"};
    if (!read_json(gcode_groups_path, gcode_groups))
        return {"ERROR: Couldn't read gcode groups"};
    if (!read_json(mcode_groups_path, mcode_groups))
        return {"ERROR: Couldn't read mcode groups"};

    ZeroPoint         zero_point{};
    MachinePointsData machine_points_data{{{"X", 0.0}, {"Y", 0.0}, {"Z", 500.0}},
                                          {{"X", 0.0}, {"Y", 0.0}, {"Z", 500.0}, {"I", 0.0}, {"J", 0.0}, {"K", 0.0}}};
    Kinematics        kinematics{{{"X", {-500., 500.}}, {"Y", {-500., 500.}}, {"Z", {0., 1000.}}},
                          20000,
                          20000,
                          10000,
                          30,
                          6,
                          1,
                          3,
                          false,
                          false,
                          "",
                          "",
                          "",
                          ""};
    CncDefaultValues  cnc_default_values{
        EMotion::RapidTraverse, // G0
        machine_tool_type == EMachineToolType::Lathe ? EWorkPlane::XZ /* G18 */ : EWorkPlane::XY /* G17 */,
        grammar_unit,
        EProgrammingType::Absolute, // G90
        EFeedMode::PerMinute,
        ERotationDirection::Right,
        EDepthProgrammingType::Absolute,
        EDrillGreturnMode::G98,
        0,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false};
    if (!ncsettings_path.empty())
    {
        bool ret{};
        std::tie(ret, machine_points_data, kinematics, cnc_default_values, zero_point) =
            read_json_ncsettings(ncsettings_path);
        if (!ret)
            return {"ERROR: Couldn't read ncsettings"};
    }

    bool evaluate_macro           = true;
    bool verify_code_groups       = true;
    bool calculate_path           = true;
    bool ncsettings_code_analysis = true;
    bool zero_point_analysis      = true;

    std::unique_ptr<AllAttributesParserBase> ap;
    switch (cnc_type)
    {
    case ECncType::Fanuc:
    case ECncType::Haas:
    case ECncType::Makino:
    case ECncType::Generic: {
        EFanucParserType fanuc_parser_type = EFanucParserType::FanucMill;
        if (cnc_type == ECncType::Fanuc)
        {
            if (machine_tool_type == EMachineToolType::Lathe)
                fanuc_parser_type = EFanucParserType::FanucLathe;
        }
        else if (cnc_type == ECncType::Haas)
        {
            if (machine_tool_type == EMachineToolType::Lathe)
                fanuc_parser_type = EFanucParserType::HaasLathe;
            else if (machine_tool_type == EMachineToolType::Mill)
                fanuc_parser_type = EFanucParserType::HaasMill;
        }
        else if (cnc_type == ECncType::Makino)
        {
            if (machine_tool_type == EMachineToolType::Mill)
                fanuc_parser_type = EFanucParserType::MakinoMill;
        }
        else if (cnc_type == ECncType::Generic)
        {
            if (machine_tool_type == EMachineToolType::Lathe)
                fanuc_parser_type = EFanucParserType::GenericLathe;
            else if (machine_tool_type == EMachineToolType::Mill)
                fanuc_parser_type = EFanucParserType::GenericMill;
        }

        ap = std::make_unique<fanuc::AllAttributesParser>(fanuc::AllAttributesParser(
            std::move(word_grammar), std::move(operations), std::move(gcode_groups), std::move(mcode_groups),
            {evaluate_macro, verify_code_groups, calculate_path, ncsettings_code_analysis, zero_point_analysis},
            {ELanguage::Polish}, fanuc_parser_type));

        break;
    }

    case ECncType::Heidenhain: {
        ap = std::make_unique<heidenhain::AllAttributesParser>(ParserSettings{evaluate_macro, verify_code_groups,
                                                                              calculate_path, ncsettings_code_analysis,
                                                                              zero_point_analysis},
                                                               OtherSettings{ELanguage::Polish});
        break;
    }
    }

    ap->set_ncsettings(machine_tool_type, std::move(machine_points_data), std::move(kinematics),
                       std::move(cnc_default_values), std::move(zero_point));

    size_t                   line_nbr{};
    size_t                   line_err{};
    std::string              data;
    std::string              text;
    const std::string        line_str("line 1");
    double                   prev_time_total{};
    std::stringstream        ss(code);
    std::vector<std::string> messages;
    while (std::getline(ss, data))
    {
        ++line_nbr;

        boost::algorithm::trim(data);
        if (data.empty())
            continue;

        bool                                  ret{};
        std::string                           message;
        std::unique_ptr<AttributeVariantData> value;
        switch (cnc_type)
        {
        case ECncType::Fanuc:
        case ECncType::Haas:
        case ECncType::Makino:
        case ECncType::Generic:
            value = std::make_unique<fanuc::FanucAttributeData>();
            break;
        case ECncType::Heidenhain:
            value = std::make_unique<heidenhain::HeidenhainAttributeData>();
            break;
        }

        if (convert_length)
            ret = ap->convert_length(static_cast<int>(line_nbr), data, *value, message,
                                     single_line_output ? true : false, unit_conversion_type);
        else if (calculate_path_time)
            ret = ap->parse(static_cast<int>(line_nbr), data, *value, message, single_line_output ? true : false);
        else if (rotate)
            ret = ap->rotate_axes(static_cast<int>(line_nbr), data, *value, message, single_line_output ? true : false,
                                  axes_rotating_option);
        else
            ret = ap->parse(static_cast<int>(line_nbr), data, message, single_line_output ? true : false);

        if (!ret)
        {
            ++line_err;
            auto pos = message.find(line_str);
            if (pos != std::string::npos)
            {
                message.replace(pos, line_str.size(), "line " + std::to_string(line_nbr));
            }
            else
            {
                if (message.empty())
                {
                    message = data;
                    message = std::to_string(line_nbr) + R"(: ')" + message + R"(')";
                }
                else if (message[0] == '1')
                {
                    message = std::to_string(line_nbr) + message.substr(1);
                }
                else
                {
                    message = std::to_string(line_nbr) + ": " + message;
                }
            }
            messages.push_back(message);
        }
        if (convert_length || calculate_path_time || rotate)
        {
            switch (cnc_type)
            {
            case ECncType::Fanuc:
            case ECncType::Haas:
            case ECncType::Makino:
            case ECncType::Generic:
                fanuc::fill_parsed_values(static_cast<fanuc::FanucAttributeData&>(*value).value, text);
                break;
            case ECncType::Heidenhain:
                heidenhain::fill_parsed_values(static_cast<heidenhain::HeidenhainAttributeData&>(*value).value, text);
                break;
            }
        }
        if (calculate_path_time)
        {
            auto pr = ap->get_path_result();
            auto tr = ap->get_time_result();
            if (tr.total != prev_time_total || pr.fast_motion != 0 || pr.work_motion != 0)
            {
                prev_time_total = tr.total;
                std::ostringstream ostr;
                ostr << "\t(" << std::fixed << std::setprecision(2) << "PathTotal=" << pr.total
                     << ", TimeTotal=" << tr.total << ", T" << pr.tool_id << "=" << pr.tool_total
                     << ", PathFastMotion=" << pr.fast_motion << ", TimeFastMotion=" << tr.fast_motion
                     << ", PathWorkMotion=" << pr.work_motion << ", TimeWorkMotion=" << tr.work_motion << ")";
                text += ostr.str().c_str();
            }
        }
        if (convert_length || calculate_path_time || rotate)
            text += "\n";
    }

    // std::cout << line_nbr << " lines parsed, " << line_err << " error(s) found!" << std::endl;

    // if (convert_length || calculate_path_time || rotate)
    // {
    //     message_all += text;
    // }

    return messages;
}
