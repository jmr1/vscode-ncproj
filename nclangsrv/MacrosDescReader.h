#pragma once

#include <map>
#include <string>
#include <vector>

namespace nclangsrv {

class Logger;
class MacrosDescReader
{
public:
    MacrosDescReader(const std::string& path, Logger* logger);

    bool read();

    using MacroDescPair   = std::pair<std::string, std::string>;
    using MacroCodesRange = std::pair<int, int>;

    MacroDescPair getDesc(const std::string& code) const;

    const std::map<std::string, MacroDescPair>& getDesc() const
    {
        return mData;
    }
    const std::vector<std::string>& getMacros() const
    {
        return mMacros;
    }

private:
    void readRanges();

    std::string                                          mPath;
    Logger*                                              mLogger;
    std::vector<std::pair<MacroCodesRange, std::string>> mRanges;
    std::map<std::string, MacroDescPair>                 mData;
    std::vector<std::string>                             mMacros;
    bool                                                 mRead{};
};

} // namespace nclangsrv