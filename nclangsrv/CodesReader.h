#pragma once

#include <map>
#include <string>

#include <GeneralParserDefines.h>

namespace nclangsrv {

class CodesReader
{
public:
    CodesReader(const std::string& path);

    bool read();

    const std::map<std::string, std::string>& getDesc() const
    {
        return mData;
    }
    const std::vector<std::string>& getCodes() const
    {
        return mCodes;
    }

private:
    std::string                        mPath;
    std::map<std::string, std::string> mData;
    std::vector<std::string>           mCodes;
    bool                               mRead{};
};

} // namespace nclangsrv