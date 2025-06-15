#include "stdafx.h"

#include "JsonMessageHandler.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <util.h>

#include "Logger.h"
#include "NCSettingsReader.h"

namespace fs = std::filesystem;

#ifdef _WIN64
#define MSG_ENDL "\n"
#else
#define MSG_ENDL "\r\n"
#endif

namespace nclangsrv {

namespace {

rapidjson::Value makeRange(int line, size_t from, size_t to, rapidjson::Document::AllocatorType& a)
{
    rapidjson::Value start(rapidjson::kObjectType);
    start.AddMember("line", line, a);
    start.AddMember("character", from, a);

    rapidjson::Value end(rapidjson::kObjectType);
    end.AddMember("line", line, a);
    end.AddMember("character", to, a);

    rapidjson::Value range(rapidjson::kObjectType);
    range.AddMember("start", start, a);
    range.AddMember("end", end, a);

    return range;
}

rapidjson::Value makeRange(int line_from, size_t char_from, int line_to, size_t char_to,
                           rapidjson::Document::AllocatorType& a)
{
    rapidjson::Value start(rapidjson::kObjectType);
    start.AddMember("line", line_from, a);
    start.AddMember("character", char_from, a);

    rapidjson::Value end(rapidjson::kObjectType);
    end.AddMember("line", line_to, a);
    end.AddMember("character", char_to, a);

    rapidjson::Value range(rapidjson::kObjectType);
    range.AddMember("start", start, a);
    range.AddMember("end", end, a);

    return range;
}

rapidjson::Value prepareMacroDescDiagnostic(const std::string& macroDescErr, rapidjson::Document::AllocatorType& a)
{
    std::string file;
    int         line{};
    std::string msg;

    const std::regex r("(.*)\\((\\d+)\\):\\s(.*)");
    std::smatch      m;
    if (std::regex_search(macroDescErr, m, r) and m.size() == 4)
    {
        file = m[1].str();
        line = std::stoi(m[2].str());
        msg  = m[3].str();
    }
    else
    {
        file = macroDescErr;
        line = 1;
        msg  = macroDescErr;
    }

    rapidjson::Value diagnostic(rapidjson::kObjectType);
    {
        diagnostic.AddMember("severity", 1, a); // Error, can be omitted
        diagnostic.AddMember("range", makeRange(0, 0, 0, a), a);

        std::string      msgTitle("Problem in user macro descriptions file");
        rapidjson::Value messageTitle;
        messageTitle.SetString(msgTitle.c_str(), static_cast<rapidjson::SizeType>(msgTitle.size()), a);
        diagnostic.AddMember("message", messageTitle, a);

        rapidjson::Value relatedInformations(rapidjson::kArrayType);

        rapidjson::Value relatedInformation(rapidjson::kObjectType);

        rapidjson::Value message;
        message.SetString(msg.c_str(), static_cast<rapidjson::SizeType>(msg.size()), a);
        relatedInformation.AddMember("message", message, a);

        rapidjson::Value location(rapidjson::kObjectType);

        rapidjson::Value uri;
        uri.SetString(file.c_str(), static_cast<rapidjson::SizeType>(file.size()), a);
        location.AddMember("uri", uri, a);
        location.AddMember("range", makeRange(line - 1, 0, 0, a), a);

        relatedInformation.AddMember("location", location, a);

        relatedInformations.PushBack(relatedInformation, a);

        diagnostic.AddMember("relatedInformation", relatedInformations, a);
    }

    return diagnostic;
}

void hoverMakeResult(const std::string& contents, int line, size_t from, size_t to, rapidjson::Value& result,
                     rapidjson::Document::AllocatorType& a)
{
    rapidjson::Value data;
    data.SetString(contents.c_str(), static_cast<rapidjson::SizeType>(contents.size()), a);
    result.AddMember("contents", data, a);

    result.AddMember("range", makeRange(line, from, to, a), a);
}

std::string searchPattern(const std::regex& r, const std::string& contentLine, int character, size_t& from, size_t& to)
{
    size_t      pos{};
    std::string strLine = contentLine;
    std::smatch m;
    while (std::regex_search(strLine, m, r))
    {
        if (character >= static_cast<int>(pos + m.position()) &&
            character < static_cast<int>(pos + m.position() + m.str().size()))
        {
            from = pos + m.position();
            to   = pos + m.position() + m.str().size();
            break;
        }
        pos += m.position() + m.str().size();
        strLine = m.suffix();
    }

    return m.str();
}

std::string extractCode(const std::string& data)
{
    std::string code = data;
    while (!code.empty() && code.size() > 1 && code[0] == '0')
        code = code.substr(1);
    return code;
}

std::string replaceAll(const std::string& str1, const std::string& from, const std::string& to)
{
    std::string str       = str1;
    size_t      start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return str;
}

std::string markdownFormatHover(const std::string& contents, const std::string& title, const std::string& description)
{
    return "**" + contents + ": " + title + "** \n\n---\n\n" + replaceAll(description, "\n", "\n\n");
}

std::string decode_uri(const std::string& uri)
{
    std::ostringstream decoded;
    for (size_t i = 0; i < uri.length(); ++i)
    {
        if (uri[i] == '%' && i + 2 < uri.length())
        {
            std::istringstream iss(uri.substr(i + 1, 2));
            int                hex = 0;
            if (iss >> std::hex >> hex)
            {
                decoded << static_cast<char>(hex);
                i += 2;
            }
            else
            {
                decoded << '%'; // leave it as-is if decode fails
            }
        }
        else
        {
            decoded << uri[i];
        }
    }
    return decoded.str();
}

fs::path sanitizeFileUri(const std::string& uri)
{
    const std::string prefix = "file://";

    std::string filepath = uri.rfind(prefix, 0) == 0 ? uri.substr(prefix.length()) : uri;
    filepath             = decode_uri(filepath);

#ifdef _WIN64
    // Fix paths like "/z:/..." -> "z:/..."
    if (filepath.size() > 2 && filepath[0] == '/' && filepath[2] == ':')
    {
        filepath = filepath.substr(1);
    }
#endif

    fs::path path(filepath);
    if (!fs::exists(path))
    {
        throw std::runtime_error("File does not exist: " + path.string());
    }
    if (!fs::is_regular_file(path))
    {
        throw std::runtime_error("Not a regular file: " + path.string());
    }

    return path;
}

std::vector<std::string> readFileLines(const std::string& uri)
{
    auto path = sanitizeFileUri(uri);

    std::ifstream file(path);
    if (!file)
    {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    std::vector<std::string> lines;
    std::string              line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back(); // Normalize Windows line endings
        }
        lines.push_back(std::move(line));
    }

    return lines;
}

std::vector<std::string> readFileLines(const std::string& uri, int lineFrom, int charFrom, int lineTo, int charTo)
{
    if (lineFrom < 0 || lineTo < 0 || lineFrom > lineTo)
    {
        throw std::invalid_argument("Invalid line range specified");
    }

    if (charFrom < 0 || charTo < 0 || (lineFrom == lineTo && charFrom > charTo))
    {
        throw std::invalid_argument("Invalid character range specified");
    }

    auto path = sanitizeFileUri(uri);

    std::ifstream file(path);
    if (!file)
    {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    int                      cnt{};
    std::vector<std::string> lines;
    std::string              line;
    while (std::getline(file, line))
    {
        if (cnt < lineFrom)
        {
            ++cnt;
            continue; // Skip lines before lineFrom
        }
        if (cnt > lineTo)
        {
            break; // Stop reading after lineTo
        }
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back(); // Normalize Windows line endings
        }
        if (cnt == lineFrom && charFrom > 0)
        {
            if (charFrom >= static_cast<int>(line.size()))
            {
                line.clear(); // If charFrom is beyond the line length, clear the line
            }
            else
            {
                line = line.substr(charFrom); // Trim to charFrom
            }
        }
        if (cnt == lineTo && cnt == lineFrom && charTo > 0)
        {
            if (charTo - charFrom < static_cast<int>(line.size()))
            {
                line = line.substr(0, charTo - charFrom); // Trim to charTo
            }
        }
        else if (cnt == lineTo && charTo > 0)
        {
            if (charTo < static_cast<int>(line.size()))
            {
                line = line.substr(0, charTo); // Trim to charTo
            }
        }
        ++cnt;
        if (line.empty())
            continue; // Skip empty lines
        lines.push_back(std::move(line));
    }

    return lines;
}

std::string parserType_str(parser::EFanucParserType parserType)
{
    std::ostringstream ostr;
    ostr << parserType;
    return ostr.str();
}

std::string getLangPrefix(parser::ELanguage lang)
{
    std::string lang_prefix("en");
    switch (lang)
    {
    case parser::ELanguage::English:
        lang_prefix = "en";
        break;
    case parser::ELanguage::Polish:
        lang_prefix = "pl";
        break;
    }

    return lang_prefix;
}

} // namespace

#define LOGGER (*mLogger)()

JsonMessageHandler::JsonMessageHandler(Logger* logger, const std::string& rootPath, NCSettingsReader& ncSettingsReader,
                                       bool calculatePathTime, const std::string& macrosDescUserPath,
                                       parser::ELanguage language /*= parser::ELanguage::English*/)
    : mLogger(logger)
    , mRootPath(rootPath)
    , mNcSettingsReader(ncSettingsReader)
    , mParser(logger, rootPath, ncSettingsReader, calculatePathTime)
    , mMacrosDescUserPath(macrosDescUserPath)
    , mLanguage(language)
{
}

bool JsonMessageHandler::parse(const std::string& json)
{
    int32_t     id{};
    std::string method;

    rapidjson::Document d;

    rapidjson::ParseResult parse_result = d.Parse(json.c_str());
    if (not parse_result)
    {
        if (mLogger)
            LOGGER << "JsonMessageHandler::" << __func__
                   << ": JSON parse error: " << rapidjson::GetParseErrorFunc(parse_result.Code()) << "("
                   << parse_result.Offset() << ")" << std::endl;
    }

    if (d.HasMember("id"))
    {
        if (d["id"].IsInt())
            id = d["id"].GetInt();
        else if (d["id"].IsString())
            id = atoi(d["id"].GetString());
    }

    if (d.HasMember("method"))
    {
        method = std::string{d["method"].GetString(), d["method"].GetStringLength()};
    }
    else
    {
        if (mLogger)
            LOGGER << "JsonMessageHandler::" << __func__ << ": no method!" << std::endl;
    }

    if (mLogger)
        LOGGER << "JsonMessageHandler::" << __func__ << ": id:" << id << ", method:" << method << std::endl;

    if (method == "initialize")
    {
        initialize(id);
    }
    else if (method == "initialized")
    {
        initialized();
    }
    else if (method == "workspace/didChangeConfiguration")
    {
        workspace_didChangeConfiguration(d);
    }
    else if (method == "textDocument/didOpen")
    {
        textDocument_didOpen(d);
    }
    else if (method == "textDocument/didChange")
    {
        textDocument_didChange(d);
    }
    else if (method == "textDocument/didClose")
    {
        textDocument_didClose(d);
    }
    else if (method == "textDocument/completion")
    {
        textDocument_completion(id);
    }
    else if (method == "completionItem/resolve")
    {
        completionItem_resolve(d);
    }
    else if (method == "textDocument/hover")
    {
        textDocument_hover(d);
    }
    else if (method == "textDocument/formatting")
    {
        textDocument_formatting(d);
    }
    else if (method == "textDocument/rangeFormatting")
    {
        textDocument_rangeFormatting(d);
    }
    else if (method == "textDocument/codeLens")
    {
        textDocument_codeLens(d);
    }
    else if (method == "codeLens/resolve")
    {
        codeLens_resolve(d);
    }
    else if (method == "$/cancelRequest")
    {
        cancelRequest();
    }
    else if (method == "shutdown")
    {
        shutdown(id);
    }
    else if (method == "exit")
    {
        mExit = true;
    }
    else
    {
        if (mLogger)
            LOGGER << "JsonMessageHandler::" << __func__ << ": WRN: No handler for method: " << method << std::endl;
    }

    return true;
}

void JsonMessageHandler::initialize(int32_t id)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", id, a);

    rapidjson::Value result(rapidjson::kObjectType);
    {
        rapidjson::Value capabilities(rapidjson::kObjectType);
        {
            rapidjson::Value textDocumentSync(rapidjson::kObjectType);
            {
                textDocumentSync.AddMember("openClose", true, a);
                textDocumentSync.AddMember(
                    "change", 1, a); // Full: Documents are synced by always sending the full content of the document.
            }

            rapidjson::Value completionProvider(rapidjson::kObjectType);
            {
                completionProvider.AddMember("resolveProvider", true, a);
            }

            rapidjson::Value codeLensProvider(rapidjson::kObjectType);
            {
                codeLensProvider.AddMember("resolveProvider", true, a);
            }

            capabilities.AddMember("textDocumentSync", textDocumentSync, a);
            capabilities.AddMember("completionProvider", completionProvider, a);
            capabilities.AddMember("hoverProvider", true, a);
            capabilities.AddMember("codeLensProvider", codeLensProvider, a);
            capabilities.AddMember("documentFormattingProvider", true, a);
            capabilities.AddMember("documentRangeFormattingProvider", true, a);
        }

        result.AddMember("capabilities", capabilities, a);
    }

    d.AddMember("result", result, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

void JsonMessageHandler::initialized()
{
}

void JsonMessageHandler::workspace_didChangeConfiguration(const rapidjson::Document& request)
{
}

void JsonMessageHandler::textDocument_didOpen(const rapidjson::Document& request)
{
    const auto& params       = request["params"];
    const auto& textDocument = params["textDocument"];

    textDocument_publishDiagnostics(textDocument["uri"].GetString(), textDocument["text"].GetString());
}

void JsonMessageHandler::textDocument_didChange(const rapidjson::Document& request)
{
    const auto& params         = request["params"];
    const auto& textDocument   = params["textDocument"];
    const auto& contentChanges = params["contentChanges"];

    textDocument_publishDiagnostics(textDocument["uri"].GetString(), contentChanges[0]["text"].GetString());
}

void JsonMessageHandler::textDocument_didClose(const rapidjson::Document& request)
{
    const auto& params       = request["params"];
    const auto& textDocument = params["textDocument"];

    mFileContexts.erase(textDocument["uri"].GetString());
}

void JsonMessageHandler::fetch_gCodesDesc()
{
    if (!mGCodes.get())
    {
        auto fsRootPath = fs::path(mRootPath);
        auto descPath   = fs::canonical(fsRootPath / fs::path("conf") /
                                        fs::path(parserType_str(mNcSettingsReader.getFanucParserType())) /
                                        fs::path("desc") / fs::path("gcode_desc_" + getLangPrefix(mLanguage) + ".json"))
                            .string();
        mGCodes = std::make_unique<CodesReader>(descPath, mLogger);
        mGCodes->read();

        for (const auto& v : mGCodes->getCodes())
            mSuggestions.push_back("G" + v);
    }
}

void JsonMessageHandler::fetch_mCodesDesc()
{
    if (!mMCodes.get())
    {
        auto fsRootPath = fs::path(mRootPath);
        auto descPath   = fs::canonical(fsRootPath / fs::path("conf") /
                                        fs::path(parserType_str(mNcSettingsReader.getFanucParserType())) /
                                        fs::path("desc") / fs::path("mcode_desc_" + getLangPrefix(mLanguage) + ".json"))
                            .string();
        mMCodes = std::make_unique<CodesReader>(descPath, mLogger);
        mMCodes->read();

        for (const auto& v : mMCodes->getCodes())
            mSuggestions.push_back("M" + v);
    }
}

void JsonMessageHandler::fetch_macrosDesc()
{
    if (!mMacrosDesc.get())
    {
        auto fsRootPath = fs::path(mRootPath);
        auto descPath   = fs::canonical(fsRootPath / fs::path("conf") /
                                        fs::path(parserType_str(mNcSettingsReader.getFanucParserType())) /
                                        fs::path("desc") / fs::path("macros_desc_" + getLangPrefix(mLanguage) + ".json"))
                            .string();
        mMacrosDesc = std::make_unique<MacrosDescReader>(descPath, mMacrosDescUserPath, mLogger);
        mMacrosDesc->read();

        for (const auto& v : mMacrosDesc->getMacros())
            mSuggestions.push_back("#" + v);
    }
}

void JsonMessageHandler::textDocument_completion(int32_t id)
{
    fetch_gCodesDesc();
    fetch_mCodesDesc();

    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", id, a);

    rapidjson::Value result(rapidjson::kArrayType);
    {
        int cnt{};
        for (const auto& value : mSuggestions)
        {
            rapidjson::Value entry(rapidjson::kObjectType);
            rapidjson::Value label;
            label.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.size()), a);
            entry.AddMember("label", label, a);
            entry.AddMember("kind", 15, a);
            entry.AddMember("insertTextFormat", 2, a);
            entry.AddMember("data", ++cnt, a);
            result.PushBack(entry, a);
        }
    }

    d.AddMember("result", result, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

void JsonMessageHandler::completionItem_resolve(const rapidjson::Document& request)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", request["id"].GetInt(), a);

    rapidjson::Value result(rapidjson::kObjectType);
    {
        if (request.HasMember("params"))
        {
            const auto& params = request["params"];

            rapidjson::Value value(rapidjson::kObjectType);
            value.CopyFrom(params["label"], a);
            result.AddMember("label", value, a);

            if (params.HasMember("insertTextFormat"))
            {
                rapidjson::Value value(rapidjson::kObjectType);
                value.CopyFrom(params["insertTextFormat"], a);
                result.AddMember("insertTextFormat", value, a);
            }

            if (params.HasMember("kind"))
            {
                rapidjson::Value value(rapidjson::kObjectType);
                value.CopyFrom(params["kind"], a);
                result.AddMember("kind", value, a);
            }

            if (params.HasMember("data"))
            {
                rapidjson::Value value(rapidjson::kObjectType);
                value.CopyFrom(params["data"], a);
                result.AddMember("data", value, a);
            }

            std::string       desc;
            std::string       title;
            const std::string label(params["label"].GetString(), params["label"].GetStringLength());
            if (!label.empty())
            {
                if (label[0] == 'G' || label[0] == 'g')
                {
                    auto it = mGCodes->getDesc().find(label.substr(1));
                    if (it != mGCodes->getDesc().cend())
                    {
                        title = it->second.first;
                        desc  = it->second.second;
                    }
                }
                else if (label[0] == 'M' || label[0] == 'm')
                {
                    auto it = mMCodes->getDesc().find(label.substr(1));
                    if (it != mMCodes->getDesc().cend())
                    {
                        title = it->second.first;
                        desc  = it->second.second;
                    }
                }
                else if (label[0] == '#')
                {
                    std::tie(title, desc) = mMacrosDesc->getDesc(label.substr(1));
                }
            }

            {
                rapidjson::Value data;
                if (!title.empty())
                {
                    data.SetString(title.c_str(), static_cast<rapidjson::SizeType>(title.size()), a);
                }
                result.AddMember("detail", data, a);
            }

            {
                rapidjson::Value data;
                if (!desc.empty())
                {
                    data.SetString(desc.c_str(), static_cast<rapidjson::SizeType>(desc.size()), a);
                }
                result.AddMember("documentation", data, a);
            }
        }
    }

    d.AddMember("result", result, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

void JsonMessageHandler::textDocument_hover(const rapidjson::Document& request)
{
    const auto& params       = request["params"];
    const auto& position     = params["position"];
    const int   line         = position["line"].GetInt();
    const int   character    = position["character"].GetInt();
    const auto& textDocument = params["textDocument"];

    const std::string uri = textDocument["uri"].GetString();

    std::string               strLine;
    parser::fanuc::macro_map* macroMap{};

    auto it = mFileContexts.find(uri);
    if (it == mFileContexts.cend())
    {
        if (mLogger)
        {
            LOGGER << "JsonMessageHandler::" << __func__ << ": Couldn't find content for uri: " << uri << std::endl;
            mLogger->flush();
        }
    }
    else if (it->second.contentLines.size() <= static_cast<size_t>(line))
    {
        if (mLogger)
        {
            LOGGER << "JsonMessageHandler::" << __func__
                   << ": Requested line number does not exist. Line number: " << line
                   << ", total number of lines: " << it->second.contentLines.size() << std::endl;
            mLogger->flush();
        }
    }
    else
    {
        strLine  = it->second.contentLines[line];
        macroMap = &it->second.macroMap;
    }

    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", request["id"].GetInt(), a);

    rapidjson::Value result(rapidjson::kObjectType);
    {
        size_t     from{};
        size_t     to{};
        std::regex r("[GgMm]\\d+\\.?\\d?");

        std::string contents = searchPattern(r, strLine, character, from, to);

        if (!contents.empty())
        {
            fetch_gCodesDesc();
            fetch_mCodesDesc();

            if (contents[0] == 'G' || contents[0] == 'g')
            {
                std::string code = extractCode(contents.substr(1));

                auto it = mGCodes->getDesc().find(code);
                if (it != mGCodes->getDesc().cend())
                    contents = markdownFormatHover(contents, it->second.first, it->second.second);
            }
            else if (contents[0] == 'M' || contents[0] == 'm')
            {
                std::string code = extractCode(contents.substr(1));

                auto it = mMCodes->getDesc().find(code);
                if (it != mMCodes->getDesc().cend())
                    contents = markdownFormatHover(contents, it->second.first, it->second.second);
            }

            if (!contents.empty())
                hoverMakeResult(contents, line, from, to, result, a);
            else
                result.SetNull();
        }
        else if (macroMap && !macroMap->empty())
        {
            size_t from{};
            size_t to{};

            std::regex  rGM("[GgMm]#\\d+");
            std::string contents = searchPattern(rGM, strLine, character, from, to);
            if (!contents.empty() && contents.size() > 2)
            {
                std::string code = extractCode(contents.substr(2));

                auto macroId = static_cast<decltype(parser::fanuc::macro_map_key::id)>(std::stoi(code));

                auto itm = macroMap->lower_bound({macroId, line + 1});
                if (itm != macroMap->cend() && itm->first.id == macroId)
                {
                    fetch_gCodesDesc();
                    fetch_mCodesDesc();

                    auto gmcode = parser::to_string_trunc(itm->second);
                    if (contents[0] == 'G' || contents[0] == 'g')
                    {
                        contents = contents + " (G" + gmcode + ")";
                        auto it  = mGCodes->getDesc().find(gmcode);
                        if (it != mGCodes->getDesc().cend())
                            contents = markdownFormatHover(contents, it->second.first, it->second.second);
                    }
                    else if (contents[0] == 'M' || contents[0] == 'm')
                    {
                        contents = contents + " (M" + gmcode + ")";
                        auto it  = mMCodes->getDesc().find(gmcode);
                        if (it != mMCodes->getDesc().cend())
                            contents = markdownFormatHover(contents, it->second.first, it->second.second);
                    }

                    if (!contents.empty())
                        hoverMakeResult(contents, line, from, to, result, a);
                    else
                        result.SetNull();
                }
                else
                {
                    contents.clear();
                }
            }
            else
            {
                std::regex rm("#\\d+");

                std::string contents = searchPattern(rm, strLine, character, from, to);

                if (!contents.empty())
                {
                    auto code    = extractCode(contents.substr(1));
                    auto macroId = static_cast<decltype(parser::fanuc::macro_map_key::id)>(std::stoi(code));

                    bool exist{};

                    auto it = macroMap->lower_bound({macroId, line + 1});
                    if (it != macroMap->cend() && it->first.id == macroId)
                    {
                        exist    = true;
                        contents = contents + " = " + parser::to_string_trunc(it->second);
                    }

                    auto [macroTitle, macroDesc] = mMacrosDesc->getDesc(code);
                    if (not macroTitle.empty() or not macroDesc.empty())
                    {
                        exist    = true;
                        contents = markdownFormatHover(contents, macroTitle, macroDesc);
                    }

                    if (not exist)
                        contents += " = undefined";

                    if (!contents.empty())
                        hoverMakeResult(contents, line, from, to, result, a);
                    else
                        result.SetNull();
                }
                else
                {
                    result.SetNull();
                }
            }
        }
        else
        {
            result.SetNull();
        }
    }

    d.AddMember("result", result, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

void JsonMessageHandler::send_window_showMessage(MessageType type, const std::string& message)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);

    const std::string methodName("window/showMessage");
    rapidjson::Value  method;
    method.SetString(methodName.c_str(), static_cast<rapidjson::SizeType>(methodName.size()), a);
    d.AddMember("method", method, a);

    rapidjson::Value params(rapidjson::kObjectType);
    {
        params.AddMember("type", static_cast<int>(type), a); // 1 = Error, 2 = Warning, 3 = Info

        {
            rapidjson::Value value;
            value.SetString(message.c_str(), static_cast<rapidjson::SizeType>(message.size()), a);
            params.AddMember("message", value, a);
        }
    }

    d.AddMember("params", params, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

void JsonMessageHandler::textDocument_formatting(const rapidjson::Document& request)
{
    const auto&       params       = request["params"];
    const auto&       textDocument = params["textDocument"];
    const std::string uri          = textDocument["uri"].GetString();
    // const auto& options      = textDocument["options"];

    std::string              newText;
    std::string              errorMessage;
    std::vector<std::string> messages;
    std::vector<std::string> contentLines;

    try
    {
        contentLines                = readFileLines(uri);
        std::tie(messages, newText) = mParser.formatContent(contentLines);
    }
    catch (const std::exception& e)
    {
        errorMessage = e.what();
    }

    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", request["id"].GetInt(), a);

    if (!errorMessage.empty())
    {
        d.AddMember("error", rapidjson::Value(rapidjson::kObjectType), a);
        d["error"].AddMember("code", -32603, a); // Internal error
        d["error"].AddMember("message", rapidjson::Value(errorMessage.c_str(), a), a);
    }
    else
    {
        rapidjson::Value result(rapidjson::kArrayType);
        {
            rapidjson::Value entry(rapidjson::kObjectType);

            entry.AddMember(
                "range",
                makeRange(0, 0, static_cast<int>(contentLines.size()), static_cast<int>(contentLines.back().size()), a),
                a);

            {
                rapidjson::Value data;
                data.SetString(newText.c_str(), static_cast<rapidjson::SizeType>(newText.size()), a);
                entry.AddMember("newText", data, a);
            }

            result.PushBack(entry, a);
        }

        d.AddMember("result", result, a);
    }

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();

    if (!messages.empty())
        send_window_showMessage(MessageType::Warning, "Some lines could not be formatted due to syntax errors.");
}

void JsonMessageHandler::textDocument_rangeFormatting(const rapidjson::Document& request)
{
    const auto&       params         = request["params"];
    const auto&       textDocument   = params["textDocument"];
    const std::string uri            = textDocument["uri"].GetString();
    const auto&       range          = params["range"];
    const auto&       start          = range["start"];
    const int         startLine      = start["line"].GetInt();
    const int         startCharacter = start["character"].GetInt();
    const auto&       end            = range["end"];
    const int         endLine        = end["line"].GetInt();
    const int         endCharacter   = end["character"].GetInt();
    // const auto& options      = params["options"];

    std::string              newText;
    std::string              errorMessage;
    std::vector<std::string> messages;
    std::vector<std::string> contentLines;

    try
    {
        contentLines                = readFileLines(uri, startLine, startCharacter, endLine, endCharacter);
        std::tie(messages, newText) = mParser.formatContent(contentLines);
    }
    catch (const std::exception& e)
    {
        errorMessage = e.what();
    }

    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", request["id"].GetInt(), a);

    if (!errorMessage.empty())
    {
        d.AddMember("error", rapidjson::Value(rapidjson::kObjectType), a);
        d["error"].AddMember("code", -32603, a); // Internal error
        d["error"].AddMember("message", rapidjson::Value(errorMessage.c_str(), a), a);
    }
    else
    {
        rapidjson::Value result(rapidjson::kArrayType);
        {
            rapidjson::Value entry(rapidjson::kObjectType);

            entry.AddMember("range", makeRange(startLine, startCharacter, endLine, endCharacter, a), a);

            {
                rapidjson::Value data;
                data.SetString(newText.c_str(), static_cast<rapidjson::SizeType>(newText.size()), a);
                entry.AddMember("newText", data, a);
            }

            result.PushBack(entry, a);
        }

        d.AddMember("result", result, a);
    }

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();

    if (!messages.empty())
        send_window_showMessage(MessageType::Warning, "Some lines could not be formatted due to syntax errors.");
}

void JsonMessageHandler::textDocument_codeLens(const rapidjson::Document& request)
{
    const auto& params       = request["params"];
    const auto& textDocument = params["textDocument"];

    const std::string uri = textDocument["uri"].GetString();

    PathTimeResult* pathTimeResult{};

    auto it = mFileContexts.find(uri);
    if (it == mFileContexts.cend())
    {
        if (mLogger)
        {
            LOGGER << "JsonMessageHandler::" << __func__ << ": Couldn't find content for uri: " << uri << std::endl;
            mLogger->flush();
        }
    }
    else
    {
        mCurrentUri    = uri;
        pathTimeResult = &it->second.pathTimeResult;
    }

    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", request["id"].GetInt(), a);

    rapidjson::Value result(rapidjson::kArrayType);

    if (!pathTimeResult || pathTimeResult->empty())
    {
        result.SetNull();
    }
    else
    {
        for (const auto& value : *pathTimeResult)
        {
            rapidjson::Value entry(rapidjson::kObjectType);
            {
                entry.AddMember("range", makeRange(static_cast<int>(value.first), 0, 0, a), a);

                // --- optional, if not provided then codeLens/resolve request will be sent to resolve missing commands.
                // But since computation of Path/Time is in no time (already computed) then it is faster to send
                // everything in one go. Resolving one by one seems to be a performance problem on Windows which is
                // opposite to what documentation says (in this specific case when everything is already computed).
                // From documentation:
                // A code lens is _unresolved_ when no command is associated to it. For performance
                // reasons the creation of a code lens and resolving should be done in two stages.
                rapidjson::Value command(rapidjson::kObjectType);
                {
                    rapidjson::Value title;
                    title.SetString(value.second.c_str(), static_cast<rapidjson::SizeType>(value.second.size()), a);

                    rapidjson::Value cmd; // no command

                    command.AddMember("title", title, a);
                    command.AddMember("command", cmd, a);
                }
                entry.AddMember("command", command, a);
                // --- optional end
            }
            result.PushBack(entry, a);
        }
    }

    d.AddMember("result", result, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

void JsonMessageHandler::codeLens_resolve(const rapidjson::Document& request)
{
    const auto& params = request["params"];
    const auto& range  = params["range"];
    const auto& start  = range["start"];
    const int   line   = start["line"].GetInt();

    PathTimeResult* pathTimeResult{};

    auto it = mFileContexts.find(mCurrentUri);
    if (it == mFileContexts.cend())
    {
        if (mLogger)
        {
            LOGGER << "JsonMessageHandler::" << __func__ << ": Couldn't find content for uri: " << mCurrentUri
                   << std::endl;
            mLogger->flush();
        }
    }
    else
    {
        pathTimeResult = &it->second.pathTimeResult;
    }

    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", request["id"].GetInt(), a);

    rapidjson::Value result(rapidjson::kObjectType);

    if (!pathTimeResult || pathTimeResult->empty())
    {
        result.SetNull();
    }
    else
    {
        const auto it = pathTimeResult->find(line);
        if (it != pathTimeResult->cend())
        {

            rapidjson::Value r(rapidjson::kObjectType);
            r.CopyFrom(range, a);
            result.AddMember("range", r, a);

            rapidjson::Value command(rapidjson::kObjectType);
            {
                rapidjson::Value title;
                title.SetString(it->second.c_str(), static_cast<rapidjson::SizeType>(it->second.size()), a);

                rapidjson::Value cmd; // no command

                command.AddMember("title", title, a);
                command.AddMember("command", cmd, a);
            }
            result.AddMember("command", command, a);
        }
        else
        {
            result.SetNull();
        }
    }

    d.AddMember("result", result, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

void JsonMessageHandler::cancelRequest()
{
}

void JsonMessageHandler::shutdown(int32_t id)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("id", id, a);

    rapidjson::Value result;
    d.AddMember("result", result, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();

    // Because client does not send exit request leaving server orphaned
    // exit server on shutdown request
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s); // wait for client to receive the response
    mExit = true;
}

void JsonMessageHandler::textDocument_publishDiagnostics(const std::string& uri, const std::string& content)
{
    FileContext       fileContext{};
    std::string       data;
    std::stringstream ss(content);
    while (std::getline(ss, data))
    {
        fileContext.contentLines.push_back(data);
    }

    std::vector<std::string> messages;
    std::tie(messages, fileContext.macroMap, fileContext.pathTimeResult) = mParser.parse(content);

    mFileContexts.erase(uri);
    mFileContexts.emplace(std::make_pair(uri, std::move(fileContext)));

    std::string macroDescErr;
    try
    {
        fetch_macrosDesc();
    }
    catch (const json_parser_exception& e)
    {
        macroDescErr = e.what();
    }

    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& a = d.GetAllocator();

    d.AddMember("jsonrpc", "2.0", a);
    d.AddMember("method", "textDocument/publishDiagnostics", a);

    rapidjson::Value params(rapidjson::kObjectType);
    {
        {
            rapidjson::Value value;
            value.SetString(uri.c_str(), static_cast<rapidjson::SizeType>(uri.size()), a);
            params.AddMember("uri", value, a);
        }

        rapidjson::Value diagnostics(rapidjson::kArrayType);

        if (not macroDescErr.empty())
        {
            auto diagnostic = prepareMacroDescDiagnostic(macroDescErr, a);
            diagnostics.PushBack(diagnostic, a);
        }

        for (const auto& error_message : messages)
        {
            if (mLogger)
                LOGGER << "JsonMessageHandler::" << __func__ << ": error_message: [" << error_message << "]"
                       << std::endl;

            int32_t     line{};
            int32_t     character{};
            std::string result_message = error_message;

            std::string              data;
            std::vector<std::string> tokens;
            std::stringstream        ss(error_message);
            while (std::getline(ss, data, ':'))
            {
                boost::algorithm::trim(data);
                tokens.push_back(data);
                if (mLogger)
                    LOGGER << "JsonMessageHandler::" << __func__ << ": [" << data << "]" << std::endl;
            }

            if (not tokens.empty())
            {
                line = atoi(tokens[0].c_str());
                if (line > 0)
                    line--;
                if (tokens.size() > 2)
                {
                    character = atoi(tokens[1].c_str());
                    if (character > 0)
                        character--;
                    result_message = tokens[2];
                }
                else
                {
                    result_message = tokens[1];
                }
            }

            if (line != 0 or character != 0 or not result_message.empty())
            {
                rapidjson::Value diagnostic(rapidjson::kObjectType);
                {
                    diagnostic.AddMember("severity", 1, a); // Error, can be omitted
                    diagnostic.AddMember("range", makeRange(line, character, character, a), a);

                    rapidjson::Value message;
                    message.SetString(result_message.c_str(), static_cast<rapidjson::SizeType>(result_message.size()),
                                      a);
                    diagnostic.AddMember("message", message, a);
                }

                diagnostics.PushBack(diagnostic, a);
            }
        }

        params.AddMember("diagnostics", diagnostics, a);
    }

    d.AddMember("params", params, a);

    rapidjson::StringBuffer                    stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    d.Accept(writer);

    std::string response{stringBuffer.GetString()};

    if (mLogger)
    {
        LOGGER << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
               << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

} // namespace nclangsrv