#include "stdafx.h"

#include "JsonMessageHandler.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <thread>

#include <boost/algorithm/string/trim.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <util.h>

#include "NCSettingsReader.h"

namespace fs = std::filesystem;

#ifdef _MSC_VER
#define MSG_ENDL "\n"
#else
#define MSG_ENDL "\r\n"
#endif

namespace nclangsrv {

namespace {
void hoverMakeResult(const std::string& contents, int line, size_t from, size_t to, rapidjson::Value& result,
                     rapidjson::Document::AllocatorType& a)
{
    rapidjson::Value data;
    data.SetString(contents.c_str(), static_cast<rapidjson::SizeType>(contents.size()), a);
    result.AddMember("contents", data, a);

    rapidjson::Value range(rapidjson::kObjectType);

    rapidjson::Value start(rapidjson::kObjectType);
    rapidjson::Value end(rapidjson::kObjectType);

    start.AddMember("line", line, a);
    start.AddMember("character", from, a);

    end.AddMember("line", line, a);
    end.AddMember("character", to, a);

    range.AddMember("start", start, a);
    range.AddMember("end", end, a);

    result.AddMember("range", range, a);
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

} // namespace

JsonMessageHandler::JsonMessageHandler(std::ofstream* logger, const std::string& rootPath,
                                       NCSettingsReader& ncSettingsReader,
                                       parser::ELanguage language /*= parser::ELanguage::English*/)
    : mLogger(logger)
    , mRootPath(rootPath)
    , mNcSettingsReader(ncSettingsReader)
    , mParser(logger, rootPath, ncSettingsReader)
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
            *mLogger << "JsonMessageHandler::" << __func__
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
            *mLogger << "JsonMessageHandler::" << __func__ << ": no method!" << std::endl;
    }

    if (mLogger)
        *mLogger << "JsonMessageHandler::" << __func__ << ": id:" << id << ", method:" << method << std::endl;

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
            *mLogger << "JsonMessageHandler::" << __func__ << ": WRN: No handler for method: " << method << std::endl;
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
                textDocumentSync.AddMember("change", 1, a);
            }

            rapidjson::Value completionProvider(rapidjson::kObjectType);
            {
                completionProvider.AddMember("resolveProvider", true, a);
            }

            capabilities.AddMember("textDocumentSync", textDocumentSync, a);
            capabilities.AddMember("completionProvider", completionProvider, a);
            capabilities.AddMember("hoverProvider", true, a);
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
        *mLogger << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
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
        const auto         fsRootPath = fs::path(mRootPath);
        std::ostringstream ostr;
        ostr << mNcSettingsReader.getFanucParserType();
        std::string lang_prefix("en");
        switch (mLanguage)
        {
        case parser::ELanguage::English:
            lang_prefix = "en";
            break;
        case parser::ELanguage::Polish:
            lang_prefix = "pl";
            break;
        }
        const std::string descPath = fs::canonical(fsRootPath / fs::path("conf") / fs::path(ostr.str()) /
                                                   fs::path("desc") / fs::path("gcode_desc_" + lang_prefix + ".json"))
                                         .string();
        mGCodes = std::make_unique<CodesReader>(descPath);
        mGCodes->read();

        const auto& codes = mGCodes->getCodes();
        for (const auto& v : codes)
            mSuggestions.push_back("G" + v);
    }
}

void JsonMessageHandler::fetch_mCodesDesc()
{
    if (!mMCodes.get())
    {
        const auto         fsRootPath = fs::path(mRootPath);
        std::ostringstream ostr;
        ostr << mNcSettingsReader.getFanucParserType();
        std::string lang_prefix("en");
        switch (mLanguage)
        {
        case parser::ELanguage::English:
            lang_prefix = "en";
            break;
        case parser::ELanguage::Polish:
            lang_prefix = "pl";
            break;
        }
        const std::string descPath = fs::canonical(fsRootPath / fs::path("conf") / fs::path(ostr.str()) /
                                                   fs::path("desc") / fs::path("mcode_desc_" + lang_prefix + ".json"))
                                         .string();
        mMCodes = std::make_unique<CodesReader>(descPath);
        mMCodes->read();

        const auto& codes = mMCodes->getCodes();
        for (const auto& v : codes)
            mSuggestions.push_back("M" + v);
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
        *mLogger << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
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
        *mLogger << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
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
            *mLogger << "JsonMessageHandler::" << __func__ << ": Couldn't find content for uri: " << uri << std::endl;
            mLogger->flush();
        }
    }
    else if (it->second.contenLines.size() <= static_cast<size_t>(line))
    {
        if (mLogger)
        {
            *mLogger << "JsonMessageHandler::" << __func__
                     << ": Requested line number does not exist. Line number: " << line
                     << ", total number of lines: " << it->second.contenLines.size() << std::endl;
            mLogger->flush();
        }
    }
    else
    {
        strLine  = it->second.contenLines[line];
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
                    contents = contents + ": " + it->second.first;
            }
            else if (contents[0] == 'M' || contents[0] == 'm')
            {
                std::string code = extractCode(contents.substr(1));

                auto it = mMCodes->getDesc().find(code);
                if (it != mMCodes->getDesc().cend())
                    contents = contents + ": " + it->second.first;
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
                            contents = contents + ": " + it->second.first;
                    }
                    else if (contents[0] == 'M' || contents[0] == 'm')
                    {
                        contents = contents + " (M" + gmcode + ")";
                        auto it  = mMCodes->getDesc().find(gmcode);
                        if (it != mMCodes->getDesc().cend())
                            contents = contents + ": " + it->second.first;
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
                    std::string code    = extractCode(contents.substr(1));
                    auto        macroId = static_cast<decltype(parser::fanuc::macro_map_key::id)>(std::stoi(code));

                    contents = contents + " = ";
                    auto it  = macroMap->lower_bound({macroId, line + 1});
                    if (it != macroMap->cend() && it->first.id == macroId)
                        contents = contents + parser::to_string_trunc(it->second);
                    else
                        contents = contents + "undefined";

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
        *mLogger << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
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
        *mLogger << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
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
        fileContext.contenLines.push_back(data);
    }

    std::vector<std::string> messages;
    std::tie(messages, fileContext.macroMap) = mParser.parse(content);

    mFileContexts.erase(uri);
    mFileContexts.emplace(std::make_pair(uri, std::move(fileContext)));

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

        for (const auto& error_message : messages)
        {
            if (mLogger)
                *mLogger << "JsonMessageHandler::" << __func__ << ": error_message: [" << error_message << "]"
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
                    *mLogger << "JsonMessageHandler::" << __func__ << ": [" << data << "]" << std::endl;
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

                    rapidjson::Value range(rapidjson::kObjectType);
                    {
                        rapidjson::Value start(rapidjson::kObjectType);
                        start.AddMember("line", line, a);
                        start.AddMember("character", character, a);
                        range.AddMember("start", start, a);

                        rapidjson::Value end(rapidjson::kObjectType);
                        end.AddMember("line", line, a);
                        end.AddMember("character", character, a);
                        range.AddMember("end", end, a);
                    }
                    diagnostic.AddMember("range", range, a);

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
        *mLogger << "JsonMessageHandler::" << __func__ << ": Content-Length: " << response.size() << std::endl
                 << "[" << response << "]" << std::endl;
        mLogger->flush();
    }

    std::cout << "Content-Length: " << response.size() << MSG_ENDL << MSG_ENDL << response;
    std::cout.flush();
}

} // namespace nclangsrv