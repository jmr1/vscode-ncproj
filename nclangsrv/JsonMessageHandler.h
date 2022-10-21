#pragma once

#include <fstream>
#include <string>

#include <rapidjson/document.h>

namespace nclangsrv {
class JsonMessageHandler
{
public:
    JsonMessageHandler(std::ofstream* logger, const std::string& rootPath, const std::string& ncSettingsPath);

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
    void textDocument_completion(int32_t id);
    void completionItem_resolve(const rapidjson::Document& request);
    void cancelRequest();
    void shutdown(int32_t id);
    void textDocument_publishDiagnostics(const std::string& uri, const std::string& content);

private:
    std::ofstream* mLogger;
    std::string    mRootPath;
    std::string    mNcSettingsPath;
    bool           mExit{};
};

} // namespace nclangsrv