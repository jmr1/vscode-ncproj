#pragma once

#include <map>
#include <string>

#include <GeneralParserDefines.h>

namespace nclangsrv {

class Logger;
class CodesReader
{
public:
    CodesReader(const std::string& path, Logger* logger);

    bool read();

    using TitleDescPair = std::pair<std::string, std::string>;

    const std::map<std::string, TitleDescPair>& getDesc() const
    {
        return mData;
    }
    const std::vector<std::string>& getCodes() const
    {
        return mCodes;
    }

private:
    std::string                          mPath;
    Logger*                              mLogger;
    std::map<std::string, TitleDescPair> mData;
    std::vector<std::string>             mCodes;
    bool                                 mRead{};
};

} // namespace nclangsrv