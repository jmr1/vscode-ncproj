#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nclangsrv {

struct json_parser_exception : std::runtime_error
{
    explicit json_parser_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

class Logger;
class MacrosDescReader
{
public:
    MacrosDescReader(const std::string& path, const std::string& macrosDescUserPath, Logger* logger);

    bool read();

    using MacroDescPair   = std::pair<std::string, std::string>;
    using MacroCodesRange = std::pair<int, int>;

    MacroDescPair getDesc(const std::string& code) const;

    const std::unordered_map<std::string, MacroDescPair>& getDesc() const
    {
        return mData;
    }
    const std::unordered_set<std::string>& getMacros() const
    {
        return mMacros;
    }

private:
    void readJson(const std::string& path);
    void readRanges();

    std::string                                          mPath;
    std::string                                          mMacrosDescUserPath;
    Logger*                                              mLogger;
    std::vector<std::pair<MacroCodesRange, std::string>> mRanges;
    std::unordered_map<std::string, MacroDescPair>       mData;
    std::unordered_set<std::string>                      mMacros;
    bool                                                 mRead{};
};

} // namespace nclangsrv