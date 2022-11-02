#include "stdafx.h"

#include "WordGrammarReader.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

using namespace parser;

namespace nclangsrv {

namespace {

void readGrammar(pt::ptree& root, const std::string& unit, fanuc::word_map& word_grammar_map)
{

    for (const auto& u : root.get_child(unit))
    {
        word_grammar_map[u.first] = {u.second.get<decltype(fanuc::WordGrammar::word)>("word"),
                                     u.second.get<decltype(fanuc::WordGrammar::range_from)>("range_from"),
                                     u.second.get<decltype(fanuc::WordGrammar::range_to)>("range_to"),
                                     u.second.get<decltype(fanuc::WordGrammar::decimal_from)>("decimal_from"),
                                     u.second.get<decltype(fanuc::WordGrammar::decimal_to)>("decimal_to"),
                                     u.second.get<decltype(fanuc::WordGrammar::unique)>("unique"),
                                     u.second.get<decltype(fanuc::WordGrammar::word_type)>("type")};
    }
}

} // namespace

WordGrammarReader::WordGrammarReader(const std::string& path)
    : mPath(path)
{
}

bool WordGrammarReader::read()
{
    if (mRead)
        return true;

    try
    {
        pt::ptree root;
        pt::read_json(mPath, root);

        mWordGrammar.metric.clear();
        mWordGrammar.imperial.clear();
        readGrammar(root, "metric", mWordGrammar.metric);
        readGrammar(root, "imperial", mWordGrammar.imperial);

        mOperations.clear();
        for (const auto& o : root.get_child("operations"))
            mOperations.push_back(o.second.get_value<std::string>());
    }
    catch (const std::exception&)
    {
        return false;
    }

    mRead = true;

    return true;
}

} // namespace nclangsrv