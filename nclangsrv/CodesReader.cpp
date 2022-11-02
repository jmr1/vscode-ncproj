#include "stdafx.h"

#include "CodesReader.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

using namespace parser;

namespace nclangsrv {

CodesReader::CodesReader(const std::string& path)
    : mPath(path)
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
        for (const auto& pt : root.get_child("descriptions"))
        {
            mData.emplace(std::make_pair(pt.first, pt.second.get_value<std::string>()));
            mCodes.emplace_back(pt.first);
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