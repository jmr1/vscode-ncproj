#pragma once

#include "parser_export.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "AllAttributesParserDefines.h"
#include "GeneralParserDefines.h"

namespace parser {

struct FileSplitting;
struct CncDefaultValues;
struct PathResult;
struct TimeResult;

namespace fanuc {

struct file_splitter_exception : std::runtime_error
{
    explicit file_splitter_exception(const std::string& message)
        : std::runtime_error(message.c_str())
    {
    }
};

class PARSER_API FileSplitter
{
public:
    FileSplitter(const FileSplitting& file_splitting, ELanguage language)
        : file_splitting(file_splitting)
        , language(language)
    {
    }

    std::vector<std::vector<std::string>> evaluate(std::vector<std::string>&& data);
    void evaluate(const CncDefaultValues& cnc_default_values, std::vector<std::vector<AttributeVariant>>&& data,
                  const std::vector<AttributeVariant>&                     retraction_plane,
                  std::vector<std::vector<std::vector<AttributeVariant>>>& value);

    void evaluate(const CncDefaultValues& cnc_default_values, std::vector<std::vector<AttributeVariant>>&& data,
                  const std::vector<AttributeVariant>&                     retraction_plane,
                  std::vector<std::vector<std::vector<AttributeVariant>>>& value,
                  const std::vector<PathResult>&                           vec_path_result);

    void evaluate(const CncDefaultValues& cnc_default_values, std::vector<std::vector<AttributeVariant>>&& data,
                  const std::vector<AttributeVariant>&                     retraction_plane,
                  std::vector<std::vector<std::vector<AttributeVariant>>>& value,
                  const std::vector<TimeResult>&                           vec_time_result);

private:
    void evaluate_block_numbers(const CncDefaultValues&                                  cnc_default_values,
                                std::vector<std::vector<AttributeVariant>>&&             data,
                                const std::vector<AttributeVariant>&                     retraction_plane,
                                std::vector<std::vector<std::vector<AttributeVariant>>>& value);
    void evaluate_tool_change(const CncDefaultValues&                                  cnc_default_values,
                              std::vector<std::vector<AttributeVariant>>&&             data,
                              const std::vector<AttributeVariant>&                     retraction_plane,
                              std::vector<std::vector<std::vector<AttributeVariant>>>& value);

private:
    const FileSplitting& file_splitting;
    const ELanguage      language;
};

} // namespace fanuc
} // namespace parser
