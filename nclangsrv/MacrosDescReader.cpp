#include "stdafx.h"

#include "MacrosDescReader.h"

#include <regex>

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

    readRanges();

    mRead = true;

    return true;
}

void MacrosDescReader::readRanges()
{
    const std::regex r1("Range: #(\\d+)-#(\\d+)");
    const std::regex r2("Range: #(\\d+)-#(\\d+) - #(\\d+)-#(\\d+)");

    for (const auto& d : mData)
    {
        const std::string& desc = d.second.second;

        std::smatch m;
        if (std::regex_search(desc, m, r2) or std::regex_search(desc, m, r1))
        {
            bool process = m.size() % 2;
            if (not process)
                continue;
            for (size_t x = 1; x < m.size(); x += 2)
            {
                int b{};
                int e{};
                try
                {
                    b = std::stoi(m[x].str());
                    e = std::stoi(m[x + 1].str());
                }
                catch (const std::invalid_argument& e)
                {
                    LOGGER << "MacrosDescReader::" << __func__ << ": ERR: " << e.what() << std::endl;
                    mLogger->flush();
                    continue;
                }
                catch (const std::out_of_range& e)
                {
                    LOGGER << "MacrosDescReader::" << __func__ << ": ERR: " << e.what() << std::endl;
                    mLogger->flush();
                    continue;
                }

                mRanges.push_back(std::make_pair(std::make_pair(b, e), d.first));
            }
        }
    }
}

MacrosDescReader::MacroDescPair MacrosDescReader::getDesc(const std::string& code) const
{
    if (mData.count(code))
    {
        return mData.at(code);
    }

    int icode{};

    try
    {
        icode = std::stoi(code);
    }
    catch (const std::invalid_argument& e)
    {
        LOGGER << "MacrosDescReader::" << __func__ << ": ERR: " << e.what() << std::endl;
        mLogger->flush();
        return {};
    }
    catch (const std::out_of_range& e)
    {
        LOGGER << "MacrosDescReader::" << __func__ << ": ERR: " << e.what() << std::endl;
        mLogger->flush();
        return {};
    }

    auto it = std::find_if(mRanges.cbegin(), mRanges.cend(),
                           [&icode](const auto& r) { return icode > r.first.first and icode < r.first.second; });
    if (it != mRanges.cend())
    {
        if (mData.count(it->second))
            return mData.at(it->second);
    }

    return {};
}

} // namespace nclangsrv