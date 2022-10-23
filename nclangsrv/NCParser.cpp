#include "stdafx.h"

#include "NCParser.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>

#include <boost/algorithm/string/trim.hpp>

#include <fanuc/AllAttributesParser.h>
#include <heidenhain/AllAttributesParser.h>

#include "NCSettingsReader.h"

namespace fs = std::filesystem;

using namespace parser;

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

void fill_parsed_values(const std::vector<AttributeVariant>& v, std::string& text)
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

void fill_parsed_values(const std::vector<AttributeVariant>& v, std::string& text)
{
    /*for(size_t x = 0; x < v.size(); ++x)
    {
        boost::apply_visitor(AttributesVisitor(text), v[x]);
        text += " ";
    }*/
}

} // namespace heidenhain

} // namespace parser

namespace nclangsrv {

NCParser::NCParser(const std::string& rootPath, NCSettingsReader& ncSettingsReader)
    : unit_conversion_type(UnitConversionType::metric_to_imperial)
    , axes_rotating_option(AxesRotatingOption::Xrotate90degrees)
    , single_line_output(true)
    , convert_length(false)
    , calculate_path_time(false)
    , rotate(false)
    , mRootPath(rootPath)
    , mNcSettingsReader(ncSettingsReader)
    , mLanguage(ELanguage::English)
{
}

std::vector<std::string> NCParser::parse(const std::string& code)
{
    if (!mNcSettingsReader.getNcSettingsPath().empty() && !mNcSettingsReader.read())
        return {"ERROR: Couldn't read .ncsetting file"};

    const auto fanuc_parser_type = mNcSettingsReader.getFanucParserType();
    const auto machine_tool_type = mNcSettingsReader.getMachineToolType();

    if (!mWordGrammarReader.get())
    {
        const auto         fsRootPath = fs::path(mRootPath);
        std::ostringstream ostr;
        ostr << mNcSettingsReader.getFanucParserType();
        const std::string grammarPath =
            fs::canonical(fsRootPath / fs::path("conf") / fs::path(ostr.str()) / fs::path("grammar.json")).string();
        mWordGrammarReader = std::make_unique<WordGrammarReader>(grammarPath);

        if (!mWordGrammarReader->read())
            return {"ERROR: Couldn't read word grammar settings"};
    }

    if (!mGCodeGroupsReader.get())
    {
        const auto         fsRootPath = fs::path(mRootPath);
        std::ostringstream ostr;
        ostr << mNcSettingsReader.getFanucParserType();
        const std::string grammarPath =
            fs::canonical(fsRootPath / fs::path("conf") / fs::path(ostr.str()) / fs::path("gcode_groups.json"))
                .string();
        mGCodeGroupsReader = std::make_unique<CodeGroupsReader>(grammarPath);

        if (!mGCodeGroupsReader->read())
            return {"ERROR: Couldn't read gcode groups settings"};
    }

    if (!mMCodeGroupsReader.get())
    {
        const auto         fsRootPath = fs::path(mRootPath);
        std::ostringstream ostr;
        ostr << mNcSettingsReader.getFanucParserType();
        const std::string grammarPath =
            fs::canonical(fsRootPath / fs::path("conf") / fs::path(ostr.str()) / fs::path("mcode_groups.json"))
                .string();
        mMCodeGroupsReader = std::make_unique<CodeGroupsReader>(grammarPath);

        if (!mMCodeGroupsReader->read())
            return {"ERROR: Couldn't read mcode groups settings"};
    }

    auto word_grammar = mWordGrammarReader->getWordGrammar();
    auto operations   = mWordGrammarReader->getOperations();

    auto gcode_groups = mGCodeGroupsReader->getCodeGroups();
    auto mcode_groups = mMCodeGroupsReader->getCodeGroups();

    auto machine_points_data = mNcSettingsReader.getMachinePointsData();
    auto kinematics          = mNcSettingsReader.getKinematics();
    auto cnc_default_values  = mNcSettingsReader.getCncDefaultValues();
    auto zero_point          = mNcSettingsReader.getZeroPoint();

    bool evaluate_macro           = true;
    bool verify_code_groups       = true;
    bool calculate_path           = true;
    bool ncsettings_code_analysis = true;
    bool zero_point_analysis      = true;

    ECncType cnc_type = ECncType::Fanuc;

    switch (fanuc_parser_type)
    {
    case EFanucParserType::FanucLathe:
    case EFanucParserType::FanucMill:
        cnc_type = ECncType::Fanuc;
        break;
    case EFanucParserType::GenericLathe:
    case EFanucParserType::GenericMill:
        cnc_type = ECncType::Generic;
        break;
    case EFanucParserType::HaasLathe:
    case EFanucParserType::HaasMill:
        cnc_type = ECncType::Haas;
        break;
    case EFanucParserType::MakinoMill:
        cnc_type = ECncType::Makino;
        break;
    }

    std::unique_ptr<AllAttributesParserBase> ap;
    switch (cnc_type)
    {
    case ECncType::Fanuc:
    case ECncType::Haas:
    case ECncType::Makino:
    case ECncType::Generic: {
        ap = std::make_unique<fanuc::AllAttributesParser>(fanuc::AllAttributesParser(
            std::move(word_grammar), std::move(operations), std::move(gcode_groups), std::move(mcode_groups),
            {evaluate_macro, verify_code_groups, calculate_path, ncsettings_code_analysis, zero_point_analysis},
            {mLanguage}, fanuc_parser_type));

        break;
    }

    case ECncType::Heidenhain: {
        ap = std::make_unique<heidenhain::AllAttributesParser>(ParserSettings{evaluate_macro, verify_code_groups,
                                                                              calculate_path, ncsettings_code_analysis,
                                                                              zero_point_analysis},
                                                               OtherSettings{mLanguage});
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

} // namespace nclangsrv