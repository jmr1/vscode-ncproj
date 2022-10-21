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

#include "NCSettingsReader.h"

namespace pt = boost::property_tree;
namespace fs = std::filesystem;

namespace {

using namespace parser;

bool read_json(const std::string& grammar_path, const std::string& unit, fanuc::word_map& word_grammar_map,
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
    catch (const std::exception&)
    {
        return false;
    }

    return true;
}

bool read_json(const std::string& code_groups_path, fanuc::code_groups_map& code_groups)
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
    catch (const std::exception&)
    {
        return false;
    }

    return true;
}

} // namespace

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

NCParser::NCParser(const std::string& rootPath, const std::string& ncSettingsPath)
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
    , ncsettings_path(ncSettingsPath)
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
        NCSettingsReader ncSettingsReader(ncsettings_path);
        if (!ncSettingsReader.read())
            return {"ERROR: Couldn't read ncsettings"};
        machine_points_data = ncSettingsReader.getMachinePointsData();
        kinematics          = ncSettingsReader.getKinematics();
        cnc_default_values  = ncSettingsReader.getCncDefaultValues();
        zero_point          = ncSettingsReader.getZeroPoint();
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
