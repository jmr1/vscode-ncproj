#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <rapidjson/document.h>

#include <GeneralParserDefines.h>

#include "CodesReader.h"
#include "NCParser.h"

namespace nclangsrv {

class NCSettingsReader;
class Logger;
class JsonMessageHandler
{
public:
    JsonMessageHandler(Logger* logger, const std::string& rootPath, NCSettingsReader& ncSettingsReader,
                       bool calculatePathTime, parser::ELanguage language = parser::ELanguage::English);

    bool parse(const std::string& json);
    bool exit() const
    {
        return mExit;
    }

private:
    void initialize(int32_t id);
    void initialized();
    void workspace_didChangeConfiguration(const rapidjson::Document& request);
    void textDocument_didOpen(const rapidjson::Document& request);
    void textDocument_didChange(const rapidjson::Document& request);
    void textDocument_didClose(const rapidjson::Document& request);
    void textDocument_completion(int32_t id);
    void completionItem_resolve(const rapidjson::Document& request);
    void textDocument_hover(const rapidjson::Document& request);
    void textDocument_codeLens(const rapidjson::Document& request);
    void codeLens_resolve(const rapidjson::Document& request);
    void cancelRequest();
    void shutdown(int32_t id);
    void textDocument_publishDiagnostics(const std::string& uri, const std::string& content);

    void fetch_gCodesDesc();
    void fetch_mCodesDesc();

    struct FileContext
    {
        std::vector<std::string> contentLines;
        parser::fanuc::macro_map macroMap;
        PathTimeResult           pathTimeResult;
    };

    struct PathTime
    {
        std::string data;
        int         line;
        int         from;
        int         to;
    };

private:
    std::map<std::string, FileContext> mFileContexts;
    std::vector<std::string>           mSuggestions;
    Logger*                            mLogger;
    std::unique_ptr<CodesReader>       mGCodes;
    std::unique_ptr<CodesReader>       mMCodes;
    std::string                        mRootPath;
    std::string                        mCurrentUri;
    NCSettingsReader&                  mNcSettingsReader;
    NCParser                           mParser;
    parser::ELanguage                  mLanguage;
    bool                               mExit{};
};

} // namespace nclangsrv