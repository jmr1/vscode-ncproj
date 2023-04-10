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
    catch (const std::exception&)
    {
        return false;
    }

    mRead = true;

    return true;
}

} // namespace nclangsrv