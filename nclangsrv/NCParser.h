#pragma once

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <GeneralParserDefines.h>
#include <fanuc/MacroDefines.h>

#include "CodeGroupsReader.h"
#include "WordGrammarReader.h"

namespace nclangsrv {

using PathTimeResult = std::map<size_t, std::string>;

class NCSettingsReader;
class Logger;

class NCParser
{
public:
    NCParser(Logger* logger, const std::string& rootPath, NCSettingsReader& ncSettingsReader, bool calculatePathTime);

    std::tuple<std::vector<std::string>, parser::fanuc::macro_map, PathTimeResult> parse(const std::string& code);

    std::tuple<std::vector<std::string>, std::string> formatContent(const std::vector<std::string>& contentLines);

private:
    Logger*                            mLogger;
    parser::UnitConversionType         unit_conversion_type;
    parser::AxesRotatingOption         axes_rotating_option;
    bool                               single_line_output;
    bool                               convert_length;
    bool                               calculate_path_time;
    bool                               rotate;
    std::string                        mRootPath;
    NCSettingsReader&                  mNcSettingsReader;
    std::unique_ptr<WordGrammarReader> mWordGrammarReader;
    std::unique_ptr<CodeGroupsReader>  mGCodeGroupsReader;
    std::unique_ptr<CodeGroupsReader>  mMCodeGroupsReader;
    parser::ELanguage                  mLanguage;
};

} // namespace nclangsrv