#pragma once

#include <string>
#include <vector>

#include <fanuc/AllAttributesParserDefines.h>

namespace nclangsrv {

class Logger;
class WordGrammarReader
{
public:
    WordGrammarReader(const std::string& path, Logger* logger);

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
    Logger*                         mLogger;
    parser::fanuc::FanucWordGrammar mWordGrammar;
    std::vector<std::string>        mOperations;
    bool                            mRead{};
};

} // namespace nclangsrv