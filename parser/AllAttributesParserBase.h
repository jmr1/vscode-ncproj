#pragma once

#include "parser_export.h"

#include <map>
#include <string>
#include <vector>

namespace parser {

class AttributeVariantData;
struct ParserSettings;
struct MachinePointsData;
struct Kinematics;
struct ZeroPoint;
struct CncDefaultValues;
struct PathResult;
struct TimeResult;
enum class EDriverUnits;
enum class UnitConversionType;
struct RenumberSettings;
enum class AxesRotatingOption;
struct FileSplitting;
enum class EMachineToolType;

class PARSER_API AllAttributesParserBase
{
public:
    virtual ~AllAttributesParserBase()
    {
    }

    virtual bool parse(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                       bool single_line_msg, const ParserSettings& parser_settings)                   = 0;
    virtual bool parse(int line, const std::string& data, std::string& message, bool single_line_msg) = 0;
    virtual bool parse(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                       bool single_line_msg)                                                          = 0;

    virtual void set_parser_settings(const ParserSettings& parser_settings) = 0;

    virtual void set_ncsettings(EMachineToolType machine_tool_type, MachinePointsData&& machine_points_data,
                                Kinematics&& kinematics, CncDefaultValues&& cnc_default_values,
                                ZeroPoint&& zero_point) = 0;

    virtual EDriverUnits get_unit_system() const = 0;

    // virtual const macro_map& get_macro_values() const = 0;
    // virtual void reset_macro_values() = 0;

    virtual void              reset_attributes_path_calculator() = 0;
    virtual const PathResult& get_path_result() const            = 0;
    virtual const TimeResult& get_time_result() const            = 0;

    // virtual const word_range_map& get_word_range() const = 0;

    virtual bool convert_length(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg, UnitConversionType conversion_type) = 0;

    virtual bool remove_numbering(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                                  bool single_line_msg)                                    = 0;
    virtual bool renumber(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                          bool single_line_msg, const RenumberSettings& renumber_settings) = 0;
    virtual void set_renumber_start_value(unsigned int value)                              = 0;
    virtual void renumber_reset()                                                          = 0;

    virtual bool rotate_axes(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                             bool single_line_msg, AxesRotatingOption axes_rotating_options) = 0;

    // virtual bool split_file(std::vector<std::string> &&data, std::vector<std::vector<AttributeVariantData>> &value,
    //    std::string &message, bool single_line_msg, const FileSplitting &file_splitting) = 0;

    virtual bool remove_comment(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg) = 0;

    virtual bool remove_optional_block(int line, const std::string& data, AttributeVariantData& value,
                                       std::string& message, bool single_line_msg) = 0;
};

} // namespace parser
