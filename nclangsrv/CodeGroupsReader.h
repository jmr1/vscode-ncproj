#pragma once

#include <map>
#include <string>

#include <fanuc/CodeGroupsDefines.h>

namespace nclangsrv {

class CodeGroupsReader
{
public:
    explicit CodeGroupsReader(const std::string& path);

    bool read();

    parser::fanuc::code_groups_map getCodeGroups() const
    {
        return mCodeGroups;
    }

private:
    std::string                    mPath;
    parser::fanuc::code_groups_map mCodeGroups;
    bool                           mRead{};
};

} // namespace nclangsrv