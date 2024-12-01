#include "stdafx.h"

#include "NCParser.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <string_view>

#include <fanuc/AllAttributesParser.h>
#include <heidenhain/AllAttributesParser.h>

#include "Logger.h"
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

namespace {

constexpr double tolerance = 0.01;

std::string formatTime(double doubleSeconds)
{
    constexpr size_t BUFFER_SIZE = 32;
    static char      buffer[BUFFER_SIZE];

    int64_t msInt  = static_cast<int64_t>(std::round(doubleSeconds * 1000.0));
    int64_t absInt = std::abs(msInt);

    int64_t hours        = absInt / (1000 * 60 * 60);
    int64_t minutes      = (absInt / (1000 * 60)) % 60;
    int64_t secondsx     = (absInt / 1000) % 60;
    int64_t milliseconds = absInt % 1000;

    if (hours > 0)
    {
        // Include hours in the format
        std::snprintf(buffer, BUFFER_SIZE, "%s%lld:%02lld:%02lld.%03lld", (msInt < 0 ? "-" : ""), hours, minutes, secondsx,
                      milliseconds);
    }
    else
    {
        // Omit hours for shorter format
        std::snprintf(buffer, BUFFER_SIZE, "%s%lld:%02lld.%03lld", (msInt < 0 ? "-" : ""), minutes, secondsx,
                      milliseconds);
    }

    return std::string(buffer);
}

std::string formatPathTimeResult(const PathResult& pr, const TimeResult& tr, double total_work_motion_path,
                                 double total_work_motion_time, double total_fast_motion_path,
                                 double total_fast_motion_time)
{
    constexpr size_t BUFFER_SIZE = 256;
    static char      buffer[BUFFER_SIZE];
    std::string      result;
    result.reserve(512);

    if (pr.total >= tolerance)
    {
        auto written = std::snprintf(buffer, BUFFER_SIZE, " | Total path = %.2f", pr.total);
        result.append(buffer, written);
    }
    if (tr.total >= tolerance)
    {
        auto formattedTime = formatTime(tr.total);
        auto written       = std::snprintf(buffer, BUFFER_SIZE, " | Total time = %s", formattedTime.c_str());
        result.append(buffer, written);
    }
    if (pr.tool_total >= tolerance && pr.total - pr.tool_total >= tolerance)
    {
        auto written =
            std::snprintf(buffer, BUFFER_SIZE, " | T%s total path = %.2f", pr.tool_id.c_str(), pr.tool_total);
        result.append(buffer, written);
    }
    if (pr.fast_motion >= tolerance)
    {
        auto written = std::snprintf(buffer, BUFFER_SIZE, " | Total rapid path = %.2f", total_fast_motion_path);
        result.append(buffer, written);
    }
    if (tr.fast_motion >= tolerance)
    {
        auto formattedTime = formatTime(total_fast_motion_time);
        auto written       = std::snprintf(buffer, BUFFER_SIZE, " | Total rapid time = %s", formattedTime.c_str());
        result.append(buffer, written);
    }
    if (pr.work_motion >= tolerance)
    {
        auto written = std::snprintf(buffer, BUFFER_SIZE, " | Total cut path = %.2f", total_work_motion_path);
        result.append(buffer, written);
    }
    if (tr.work_motion >= tolerance)
    {
        auto formattedTime = formatTime(total_work_motion_time);
        auto written       = std::snprintf(buffer, BUFFER_SIZE, " | Total cut time = %s", formattedTime.c_str());
        result.append(buffer, written);
    }

    return result;
}

std::string_view rtrim(std::string_view line)
{
    // Trim right trailing whitespace or carriage return
    while (!line.empty() && (std::isspace(line.back()) || line.back() == '\r'))
    {
        line.remove_suffix(1);
    }
    return line;
}

std::vector<std::string_view> splitLines(const std::string& code)
{
    std::vector<std::string_view> lines;

    size_t start = 0, end = 0;
    while (end < code.size())
    {
        if (code[end] == '\n')
        {
            auto line = std::string_view(code.data() + start, end - start);
            lines.emplace_back(rtrim(line));
            start = end + 1;
        }
        ++end;
    }
    // Handle the last line (if not ending with a newline)
    if (start < code.size())
    {
        auto line = std::string_view(code.data() + start, code.size() - start);
        lines.emplace_back(rtrim(line));
    }

    return lines;
}

} // namespace

#define LOGGER (*mLogger)()

NCParser::NCParser(Logger* logger, const std::string& rootPath, NCSettingsReader& ncSettingsReader,
                   bool calculatePathTime)
    : mLogger(logger)
    , unit_conversion_type(UnitConversionType::metric_to_imperial)
    , axes_rotating_option(AxesRotatingOption::Xrotate90degrees)
    , single_line_output(true)
    , convert_length(false)
    , calculate_path_time(calculatePathTime)
    , rotate(false)
    , mRootPath(rootPath)
    , mNcSettingsReader(ncSettingsReader)
    , mLanguage(ELanguage::English)
{
}

std::tuple<std::vector<std::string>, fanuc::macro_map, PathTimeResult> NCParser::parse(const std::string& code)
{
    if (!mNcSettingsReader.getNcSettingsPath().empty() && !mNcSettingsReader.read())
        return {{"ERROR: Couldn't read .ncsetting file"}, {}, {}};

    const auto fanuc_parser_type = mNcSettingsReader.getFanucParserType();
    const auto machine_tool      = mNcSettingsReader.getMachineTool();
    const auto machine_tool_type = mNcSettingsReader.getMachineToolType();

    if (!mWordGrammarReader.get())
    {
        const auto         fsRootPath = fs::path(mRootPath);
        std::ostringstream ostr;
        ostr << mNcSettingsReader.getFanucParserType();
        const std::string grammarPath =
            fs::canonical(fsRootPath / fs::path("conf") / fs::path(ostr.str()) / fs::path("grammar.json")).string();
        if (mLogger)
            LOGGER << "NCParser::" << __func__ << ": grammarPath: " << grammarPath << std::endl;
        mWordGrammarReader = std::make_unique<WordGrammarReader>(grammarPath, mLogger);

        if (!mWordGrammarReader->read())
            return {{"ERROR: Couldn't read word grammar settings"}, {}, {}};
    }

    if (!mGCodeGroupsReader.get())
    {
        const auto         fsRootPath = fs::path(mRootPath);
        std::ostringstream ostr;
        ostr << mNcSettingsReader.getFanucParserType();
        const std::string gCodeGroupsPath =
            fs::canonical(fsRootPath / fs::path("conf") / fs::path(ostr.str()) / fs::path("gcode_groups.json"))
                .string();
        if (mLogger)
            LOGGER << "NCParser::" << __func__ << ": gCodeGroupsPath: " << gCodeGroupsPath << std::endl;
        mGCodeGroupsReader = std::make_unique<CodeGroupsReader>(gCodeGroupsPath, mLogger);

        if (!mGCodeGroupsReader->read())
            return {{"ERROR: Couldn't read gcode groups settings"}, {}, {}};
    }

    if (!mMCodeGroupsReader.get())
    {
        const auto         fsRootPath = fs::path(mRootPath);
        std::ostringstream ostr;
        ostr << mNcSettingsReader.getFanucParserType();
        const std::string mCodeGroupsPath =
            fs::canonical(fsRootPath / fs::path("conf") / fs::path(ostr.str()) / fs::path("mcode_groups.json"))
                .string();
        if (mLogger)
            LOGGER << "NCParser::" << __func__ << ": mCodeGroupsPath: " << mCodeGroupsPath << std::endl;
        mMCodeGroupsReader = std::make_unique<CodeGroupsReader>(mCodeGroupsPath, mLogger);

        if (!mMCodeGroupsReader->read())
            return {{"ERROR: Couldn't read mcode groups settings"}, {}, {}};
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
    case EFanucParserType::FanucLatheSystemA:
    case EFanucParserType::FanucLatheSystemB:
    case EFanucParserType::FanucLatheSystemC:
    case EFanucParserType::FanucMill:
    case EFanucParserType::FanucMillturnSystemA:
    case EFanucParserType::FanucMillturnSystemB:
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
    std::unique_ptr<AttributeVariantData>    value;
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

        auto apf = dynamic_cast<fanuc::AllAttributesParser*>(ap.get());
        apf->reset_macro_values();
        // apf->reset_attributes_path_calculator();

        value = std::make_unique<fanuc::FanucAttributeData>();
        break;
    }

    case ECncType::Heidenhain: {
        ap = std::make_unique<heidenhain::AllAttributesParser>(ParserSettings{evaluate_macro, verify_code_groups,
                                                                              calculate_path, ncsettings_code_analysis,
                                                                              zero_point_analysis},
                                                               OtherSettings{mLanguage});

        value = std::make_unique<heidenhain::HeidenhainAttributeData>();
        break;
    }
    }

    ap->set_ncsettings(machine_tool, machine_tool_type, std::move(machine_points_data), std::move(kinematics),
                       std::move(cnc_default_values), std::move(zero_point));

    size_t                   line_nbr{};
    size_t                   line_err{};
    std::string              text;
    const std::string        line_str("line 1");
    double                   prev_time_total{};
    double                   total_work_motion_path{};
    double                   total_work_motion_time{};
    double                   total_fast_motion_path{};
    double                   total_fast_motion_time{};
    std::vector<std::string> messages;
    PathTimeResult           pathTimeResult;
    auto                     lines = splitLines(code);

    for (auto data : lines)
    {
        ++line_nbr;

        if (data.empty())
            continue;

        bool        ret{};
        std::string message;
        switch (cnc_type)
        {
        case ECncType::Fanuc:
        case ECncType::Haas:
        case ECncType::Makino:
        case ECncType::Generic:
            static_cast<fanuc::FanucAttributeData&>(*value).value.clear();
            break;
        case ECncType::Heidenhain:
            static_cast<heidenhain::HeidenhainAttributeData&>(*value).value.clear();
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
        if (convert_length || rotate)
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
            if (tr.total != prev_time_total /*|| pr.fast_motion != 0 || pr.work_motion != 0*/)
            {
                prev_time_total = tr.total;
                if (pr.fast_motion != 0)
                    total_fast_motion_path += pr.fast_motion;
                if (tr.fast_motion != 0)
                    total_fast_motion_time += tr.fast_motion;
                if (pr.work_motion != 0)
                    total_work_motion_path += pr.work_motion;
                if (tr.work_motion != 0)
                    total_work_motion_time += tr.work_motion;

                pathTimeResult.emplace_hint(pathTimeResult.end(), line_nbr - 1,
                                            formatPathTimeResult(pr, tr, total_work_motion_path, total_work_motion_time,
                                                                 total_fast_motion_path, total_fast_motion_time));
            }
        }
    }

    fanuc::macro_map macro_m;
    switch (cnc_type)
    {
    case ECncType::Fanuc:
    case ECncType::Haas:
    case ECncType::Makino:
    case ECncType::Generic: {
        auto apf = dynamic_cast<fanuc::AllAttributesParser*>(ap.get());
        macro_m  = apf->get_macro_values();
        break;
    }

    case ECncType::Heidenhain: {
        break;
    }
    }

    return std::make_tuple(messages, macro_m, pathTimeResult);
}

} // namespace nclangsrv