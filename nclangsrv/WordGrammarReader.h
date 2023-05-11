#pragma once

#include <string>
#include <vector>

#include <fanuc/AllAttributesParserDefines.h>

namespace nclangsrv {

class WordGrammarReader
{
public:
    explicit WordGrammarReader(const std::string& path);

    bool read();

    parser::fanuc::FanucWordGrammar getWordGrammar() const
    {
        return mWordGrammar;
    }
    std::vector<std::string> getOperations() const
    {
        return mOperations;
    }

private:
    std::string                     mPath;
    parser::fanuc::FanucWordGrammar mWordGrammar;
    std::vector<std::string>        mOperations;
    bool                            mRead{};
};

} // namespace nclangsrv