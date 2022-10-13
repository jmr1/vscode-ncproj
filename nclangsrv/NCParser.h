#pragma once

#include <string>
#include <vector>

#include <GeneralParserDefines.h>

class NCParser
{
public:
    NCParser(const std::string& rootPath = "");

    void set_cnc_type(parser::ECncType value);
    void set_machine_tool_type(parser::EMachineToolType value);

    std::vector<std::string> parse(const std::string& code);

private:
    void redo_paths();

private:
    parser::ECncType           cnc_type;
    parser::EMachineToolType   machine_tool_type;
    std::string                grammar_path;
    std::string                gcode_groups_path;
    std::string                mcode_groups_path;
    parser::EDriverUnits       grammar_unit;
    parser::UnitConversionType unit_conversion_type;
    parser::AxesRotatingOption axes_rotating_option;
    bool                       single_line_output;
    bool                       convert_length;
    bool                       calculate_path_time;
    bool                       rotate;
    std::string                ncsettings_path;
};