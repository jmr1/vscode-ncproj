#pragma once

#include <map>
#include <string>

#include <fanuc/CodeGroupsDefines.h>

namespace nclangsrv {

class Logger;
class CodeGroupsReader
{
public:
    CodeGroupsReader(const std::string& path, Logger* logger);

    bool read();

    parser::fanuc::code_groups_map getCodeGroups() const
    {
        return mCodeGroups;
    }

private:
    std::string                    mPath;
    Logger*                        mLogger;
    parser::fanuc::code_groups_map mCodeGroups;
    bool                           mRead{};
};

} // namespace nclangsrv