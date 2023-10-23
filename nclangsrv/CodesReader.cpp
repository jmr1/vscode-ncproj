#include "stdafx.h"

#include "CodesReader.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Logger.h"

namespace pt = boost::property_tree;

#define LOGGER (*mLogger)()

using namespace parser;

namespace nclangsrv {

CodesReader::CodesReader(const std::string& path, Logger* logger)
    : mPath(path)
    , mLogger(logger)
{
}

bool CodesReader::read()
{
    if (mRead)
        return true;

    try
    {
        pt::ptree root;
        pt::read_json(mPath, root);

        mData.clear();
        mCodes.clear();
        for (const auto& [code, titleDesc] : root.get_child("descriptions"))
        {
            bool addSynonim = code.size() == 1 || (code.size() == 3 && code[1] == '.');
            for (const auto& [title, desc] : titleDesc)
            {
                auto [it, ret] =
                    mData.emplace(std::make_pair(code, std::make_pair(title, desc.get_value<std::string>())));
                if (addSynonim)
                    mData.emplace(std::make_pair("0" + it->first, it->second));
            }
            mCodes.emplace_back(code);
            if (addSynonim)
                mCodes.emplace_back("0" + code);
        }
    }
    catch (const pt::json_parser_error& e)
    {
        if (mLogger)
        {
            LOGGER << "CodesReader::" << __func__ << ": ERR: " << e.what() << std::endl;
            mLogger->flush();
        }
        return false;
    }
    catch (const std::exception& e)
    {
        if (mLogger)
        {
            LOGGER << "CodesReader::" << __func__ << ": ERR: " << e.what() << std::endl;
            mLogger->flush();
        }
        return false;
    }

    mRead = true;

    return true;
}

} // namespace nclangsrv