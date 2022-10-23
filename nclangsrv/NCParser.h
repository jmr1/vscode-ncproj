#pragma once

#include <memory>
#include <string>
#include <vector>

#include <GeneralParserDefines.h>

#include "CodeGroupsReader.h"
#include "WordGrammarReader.h"

namespace nclangsrv {

class NCSettingsReader;

class NCParser
{
public:
    NCParser(const std::string& rootPath, NCSettingsReader& ncSettingsReader);

    std::vector<std::string> parse(const std::string& code);

private:
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