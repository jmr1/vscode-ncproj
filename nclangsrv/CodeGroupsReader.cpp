#include "stdafx.h"

#include "CodeGroupsReader.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Logger.h"

namespace pt = boost::property_tree;

#define LOGGER (*mLogger)()

using namespace parser;

namespace nclangsrv {

CodeGroupsReader::CodeGroupsReader(const std::string& path, Logger* logger)
    : mPath(path)
    , mLogger(logger)
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
        for (const auto& [groupName, arr] : root.get_child("groups"))
        {
            for (const auto& [_, group] : arr)
            {
                mCodeGroups[{group.get<decltype(fanuc::CodeGroupValue::code)>("code"),
                             group.get<decltype(fanuc::CodeGroupValue::rest)>("rest")}]
                    .insert(groupName);
            }
        }
    }
    catch (const pt::json_parser_error& e)
    {
        if (mLogger)
        {
            LOGGER << "CodeGroupsReader::" << __func__ << ": ERR: " << e.what() << std::endl;
            mLogger->flush();
        }
        return false;
    }
    catch (const std::exception& e)
    {
        if (mLogger)
        {
            LOGGER << "CodeGroupsReader::" << __func__ << ": ERR: " << e.what() << std::endl;
            mLogger->flush();
        }
        return false;
    }

    mRead = true;

    return true;
}

} // namespace nclangsrv