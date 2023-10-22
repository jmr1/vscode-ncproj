#include "stdafx.h"

#include "MacrosDescReader.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Logger.h"

namespace pt = boost::property_tree;

#define LOGGER (*mLogger)()

namespace nclangsrv {

MacrosDescReader::MacrosDescReader(const std::string& path, Logger* logger)
    : mPath(path)
    , mLogger(logger)
{
}

bool MacrosDescReader::read()
{
    if (mRead)
        return true;

    try
    {
        pt::ptree root;
        pt::read_json(mPath, root);

        mData.clear();
        mMacros.clear();
        for (const auto& [code, titleDesc] : root.get_child("descriptions"))
        {
            for (const auto& [title, desc] : titleDesc)
                mData.emplace(std::make_pair(code, std::make_pair(title, desc.get_value<std::string>())));
            mMacros.emplace_back(code);
        }
    }
    catch (const pt::json_parser_error& e)
    {
        if (mLogger)
        {
            LOGGER << "MacrosDescReader::" << __func__ << ": ERR: " << e.what() << std::endl;
            mLogger->flush();
        }
        return false;
    }
    catch (const std::exception& e)
    {
        if (mLogger)
        {
            LOGGER << "MacrosDescReader::" << __func__ << ": ERR: " << e.what() << std::endl;
            mLogger->flush();
        }
        return false;
    }

    mRead = true;

    return true;
}

} // namespace nclangsrv