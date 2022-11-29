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
            bool addSynonim = pt.first.size() == 1 || (pt.first.size() == 3 && pt.first[1] == '.');
            for (const auto& pt2 : root.get_child(pt::ptree::path_type("descriptions/" + pt.first, '/')))
            {
                mData.emplace(std::make_pair(pt.first, std::make_pair(pt2.first, pt2.second.get_value<std::string>())));
                if (addSynonim)
                    mData.emplace(
                        std::make_pair("0" + pt.first, std::make_pair(pt2.first, pt2.second.get_value<std::string>())));
            }
            mCodes.emplace_back(pt.first);
            if (addSynonim)
                mCodes.emplace_back("0" + pt.first);
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