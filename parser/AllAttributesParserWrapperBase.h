#pragma once

#include "parser_export.h"

#include <memory>
#include <string>

namespace parser {

enum class UnitConversionType;
enum class AxesRotatingOption;
struct CncDefaultValues;
enum class FileSplittingType;
struct ZeroPoint;
struct ParserSettings;
struct RenumberSettings;
enum class EMachineTool;
enum class EMachineToolType;

struct WordValueWrapper;
struct PathResultWrapper;
struct TimeResultWrapper;
struct WordResultRange;
struct FileSplittingWrapper;

class AllAttributesParserBase;

class AllAttributesParserWrapperBase
{
public:
    AllAttributesParserWrapperBase(std::unique_ptr<AllAttributesParserBase> ptr);
    virtual ~AllAttributesParserWrapperBase()
    {
    }

    virtual bool ParseValue(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                            bool single_line_msg) = 0;

    virtual void GetMacroValues(int** macro_ids, int** macro_lines, double** macro_values, int* length) = 0;

    virtual void GetLineMacroValues(int macro_line, int** macro_ids, double** macro_values, int* length) = 0;

    virtual void ResetMacroValues() = 0;

    virtual void GetPathValues(PathResultWrapper*& path_result, TimeResultWrapper*& time_result,
                               WordResultRange** word_result_range, int* length, int* unit_system) = 0;

    virtual bool ConvertLength(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                               bool single_line_msg, UnitConversionType conversion_type) = 0;

    virtual bool RemoveNumbering(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                                 bool single_line_msg) = 0;

    virtual bool Renumber(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                          bool single_line_msg, const RenumberSettings& renumber_settings) = 0;

    virtual bool RotateAxes(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                            bool single_line_msg, AxesRotatingOption axes_rotating_options) = 0;

    virtual bool SplitFile(FileSplittingWrapper* fsw, const char** data, int data_length, WordValueWrapper** value,
                           int**  value_value_length, // value[][][x]
                           int**  value_length,       // value[][x][]
                           int*   length,             // value[x][][]
                           char** message, bool single_line_msg) = 0;

    virtual bool SplitFileString(FileSplittingWrapper* fsw, const char** data, int data_length, char*** value,
                                 int*   length, // value[x][]
                                 char** message, bool single_line_msg) = 0;

    virtual bool RemoveComment(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                               bool single_line_msg) = 0;

    virtual bool RemoveOptionalBlock(int line, const char* data, WordValueWrapper** value, int* length, char** message,
                                     bool single_line_msg) = 0;

    AllAttributesParserBase* get_parser() const
    {
        return parser.get();
    }

protected:
    std::unique_ptr<AllAttributesParserBase> parser;
};

#ifdef __cplusplus
extern "C" {
#endif

struct WordValueWrapper
{
    char* word;
    char* value;
};

struct WordResultRange
{
    char*  word;
    double value_min;
    double value;
    double value_max;
};

struct PathResultWrapper
{
    double total;
    double work_motion; // G1 G2 G3
    double fast_motion; // G0
    double tool_total;
    char*  tool_id;
};

struct TimeResultWrapper
{
    double total;
    double work_motion; // G1 G2 G3
    double fast_motion; // G0
    double tool_total;
    char*  tool_id;
};

struct RenumberSettingsWrapper
{
    char*        word;
    unsigned int step;
    unsigned int frequency;
    unsigned int max_number;
    bool         existing_only;
    bool         leading_zeroes;
};

struct AxisParametersWrapper
{
    char*  axis;
    double range_min;
    double range_max;
};

struct KinematicsWrapper
{
    double max_working_feed;
    double max_fast_feed;
    int    maximum_spindle_speed;
    int    numer_of_items_in_the_warehouse;
    int    tool_exchange_time;
    int    pallet_exchange_time;
    int    tool_measurement_time;
    bool   diameter_programming_2x;
    bool   auto_measure_after_tool_selection;
    char*  pallet_exchange_code;
    char*  pallet_exchange_code_value;
    char*  tool_measurement_code;
    char*  tool_measurement_code_value;
};

struct FileSplittingWrapper
{
    FileSplittingType type;
    int               step;
    int               time;
    int               path;
    char*             text;
    bool              rapid_traverse_only;
    char*             retraction_plane;
};

extern PARSER_API void DisposeAllAttributesParser(AllAttributesParserWrapperBase* ptr);

extern PARSER_API void SetParserSettings(AllAttributesParserWrapperBase* ptr, const ParserSettings* parser_settings);

extern PARSER_API bool Parse(AllAttributesParserWrapperBase* ptr, int line, const char* data, char** message,
                             bool single_line_msg);

extern PARSER_API void DisposeCharArray(char* data);

extern PARSER_API bool ParseValue(AllAttributesParserWrapperBase* ptr, int line, const char* data,
                                  WordValueWrapper** value, int* length, char** message, bool single_line_msg);

extern PARSER_API void DisposeWordValueWrapper(WordValueWrapper* data, int length);

extern PARSER_API void GetMacroValues(AllAttributesParserWrapperBase* ptr, int** macro_ids, int** macro_lines,
                                      double** macro_values, int* length);

extern PARSER_API void GetLineMacroValues(AllAttributesParserWrapperBase* ptr, int macro_line, int** macro_ids,
                                          double** macro_values, int* length);

extern PARSER_API void DisposeMacroValues(int* macro_ids, int* macro_lines, double* macro_values);

extern PARSER_API void ResetMacroValues(AllAttributesParserWrapperBase* ptr);

extern PARSER_API void GetPathValues(AllAttributesParserWrapperBase* ptr, PathResultWrapper*& path_result,
                                     TimeResultWrapper*& time_result, WordResultRange** word_result_range, int* length,
                                     int* unit_system);

extern PARSER_API void DisposePathValues(PathResultWrapper* path_result, TimeResultWrapper* time_result,
                                         WordResultRange* word_result_range, int length);

extern PARSER_API void ResetPathValues(AllAttributesParserWrapperBase* ptr);

extern PARSER_API bool ConvertLength(AllAttributesParserWrapperBase* ptr, int line, const char* data,
                                     WordValueWrapper** value, int* length, char** message, bool single_line_msg,
                                     UnitConversionType conversion_type);

extern PARSER_API bool RemoveNumbering(AllAttributesParserWrapperBase* ptr, int line, const char* data,
                                       WordValueWrapper** value, int* length, char** message, bool single_line_msg);

extern PARSER_API bool Renumber(AllAttributesParserWrapperBase* ptr, int line, const char* data,
                                WordValueWrapper** value, int* length, char** message, bool single_line_msg,
                                RenumberSettingsWrapper* renumber_settings);

extern PARSER_API void SetRenumberStartValue(AllAttributesParserWrapperBase* ptr, unsigned int value);

extern PARSER_API void RenumberReset(AllAttributesParserWrapperBase* ptr);

extern PARSER_API bool RotateAxes(AllAttributesParserWrapperBase* ptr, int line, const char* data,
                                  WordValueWrapper** value, int* length, char** message, bool single_line_msg,
                                  AxesRotatingOption axes_rotating_options);

extern PARSER_API void SetNCSettings(AllAttributesParserWrapperBase* ptr, EMachineTool machine_tool,
                                     EMachineToolType machine_tool_type, char** machine_base_point_default_keys,
                                     double* machine_base_point_default_vals, int machine_base_point_default_length,
                                     char** tool_exchange_point_default_keys, double* tool_exchange_point_default_vals,
                                     int tool_exchange_point_default_length, KinematicsWrapper* kinematics_wrapper,
                                     AxisParametersWrapper* kinematics_axis_parameters,
                                     int kinematics_axis_parameters_length, CncDefaultValues* cnc_default_values,
                                     ZeroPoint* zero_point);

extern PARSER_API bool SplitFile(AllAttributesParserWrapperBase* ptr, FileSplittingWrapper* fsw, const char** data,
                                 int data_length, WordValueWrapper** value,
                                 int**  value_value_length, // value[][][x]
                                 int**  value_length,       // value[][x][]
                                 int*   length,             // value[x][][]
                                 char** message, bool single_line_msg);

extern PARSER_API void DisposeSplitFile(WordValueWrapper* value,
                                        int*              value_value_length, // value[][][x]
                                        int*              value_length,       // value[][x][]
                                        int               length);                          // value[x][][]

extern PARSER_API bool SplitFileString(AllAttributesParserWrapperBase* ptr, FileSplittingWrapper* fsw,
                                       const char** data, int data_length, char*** value,
                                       int*   length, // value[x][]
                                       char** message, bool single_line_msg);

extern PARSER_API void DisposeSplitFileString(char** value, int length);

extern PARSER_API bool RemoveComment(AllAttributesParserWrapperBase* ptr, int line, const char* data,
                                     WordValueWrapper** value, int* length, char** message, bool single_line_msg);

extern PARSER_API bool RemoveOptionalBlock(AllAttributesParserWrapperBase* ptr, int line, const char* data,
                                           WordValueWrapper** value, int* length, char** message, bool single_line_msg);

#ifdef __cplusplus
}
#endif

} // namespace parser
