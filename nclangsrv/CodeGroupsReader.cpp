#include "stdafx.h"

#include "CodeGroupsReader.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

using namespace parser;

namespace nclangsrv {

CodeGroupsReader::CodeGroupsReader(const std::string& path)
    : mPath(path)
{
}

bool CodeGroupsReader::read()
{
    if (mRead)
        return true;

    try
    {
        pt::ptree root;
        pt::read_json(mPath, root);

        mCodeGroups.clear();
        for (const auto& u : root.get_child("groups"))
        {
            for (const auto& group : u.second)
            {
                mCodeGroups[{group.second.get<decltype(fanuc::CodeGroupValue::code)>("code"),
                             group.second.get<decltype(fanuc::CodeGroupValue::rest)>("rest")}]
                    .insert(u.first);
            }
        }
    }
    catch (const std::exception&)
    {
        return false;
    }

    mRead = true;

    return true;
}

} // namespace nclangsrv