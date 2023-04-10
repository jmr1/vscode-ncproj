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

    for (const auto& [word, grammar] : root.get_child(unit))
    {
        word_grammar_map[word] = {grammar.get<decltype(fanuc::WordGrammar::word)>("word"),
                                  grammar.get<decltype(fanuc::WordGrammar::range_from)>("range_from"),
                                  grammar.get<decltype(fanuc::WordGrammar::range_to)>("range_to"),
                                  grammar.get<decltype(fanuc::WordGrammar::decimal_from)>("decimal_from"),
                                  grammar.get<decltype(fanuc::WordGrammar::decimal_to)>("decimal_to"),
                                  grammar.get<decltype(fanuc::WordGrammar::unique)>("unique"),
                                  grammar.get<decltype(fanuc::WordGrammar::word_type)>("type")};
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
        for (const auto& [_, op] : root.get_child("operations"))
            mOperations.push_back(op.get_value<std::string>());
    }
    catch (const std::exception&)
    {
        return false;
    }

    mRead = true;

    return true;
}

} // namespace nclangsrv