#pragma once

#include "parser_export.h"

#include "AllAttributesParserWrapperBase.h"

#include <memory>
#include <string>

namespace parser {
namespace heidenhain {

class AllAttributesParserWrapper : public AllAttributesParserWrapperBase
{
public:
    AllAttributesParserWrapper(std::unique_ptr<AllAttributesParserBase> ptr);

    virtual bool ParseValue(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                            bool single_line_msg) override;

    virtual void GetMacroValues(int** macro_ids, int** macro_lines, double** macro_values, int* length) override;

    virtual void GetLineMacroValues(int macro_line, int** macro_ids, double** macro_values, int* length) override;

    virtual void ResetMacroValues() override;

    virtual void GetPathValues(PathResultWrapper*& path_result, TimeResultWrapper*& time_result,
                               WordResultRange** word_result_range, int* length, int* unit_system) override;

    virtual bool ConvertLength(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                               bool single_line_msg, UnitConversionType conversion_type) override;

    virtual bool RemoveNumbering(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                                 bool single_line_msg) override;

    virtual bool Renumber(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                          bool single_line_msg, const RenumberSettings& renumber_settings) override;

    virtual bool RotateAxes(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                            bool single_line_msg, AxesRotatingOption axes_rotating_options) override;

    virtual bool SplitFile(FileSplittingWrapper* fsw, const char** data, int data_length, WordValueWrapper** value,
                           int**  value_value_length, // value[][][x]
                           int**  value_length,       // value[][x][]
                           int*   length,             // value[x][][]
                           char** message, bool single_line_msg) override;

    virtual bool SplitFileString(FileSplittingWrapper* fsw, const char** data, int data_length, char*** value,
                                 int*   length, // value[x][]
                                 char** message, bool single_line_msg) override;

    virtual bool RemoveComment(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                               bool single_line_msg) override;

    virtual bool RemoveOptionalBlock(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                                     bool single_line_msg) override;
};

#ifdef __cplusplus
extern "C" {
#endif

extern PARSER_API AllAttributesParserWrapperBase*
                  CreateHeidenhainAllAttributesParser(const ParserSettings* parser_settings, const OtherSettings* other_settings);

#ifdef __cplusplus
}
#endif

} // namespace heidenhain
} // namespace parser
