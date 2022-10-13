#include "stdafx.h"

#include <string.h>

#include "AllAttributesParserBase.h"
#include "AllAttributesParserWrapperBase.h"
#include "GeneralParserDefines.h"

namespace parser {

AllAttributesParserWrapperBase::AllAttributesParserWrapperBase(std::unique_ptr<AllAttributesParserBase> ptr)
    : parser(std::move(ptr))
{
}

MachinePointsData makeMachinePointsData(int machine_base_point_default_length, char** machine_base_point_default_keys,
                                        double* machine_base_point_default_vals, int tool_exchange_point_default_length,
                                        char**  tool_exchange_point_default_keys,
                                        double* tool_exchange_point_default_vals)
{
    MachinePointsData dv;
    for (int x = 0; x < machine_base_point_default_length; ++x)
        dv.machine_base_point[machine_base_point_default_keys[x]] = machine_base_point_default_vals[x];
    for (int x = 0; x < tool_exchange_point_default_length; ++x)
        dv.tool_exchange_point[tool_exchange_point_default_keys[x]] = tool_exchange_point_default_vals[x];

    return dv;
}

void DisposeAllAttributesParser(AllAttributesParserWrapperBase* ptr)
{
    if (ptr)
        delete ptr;
}

void SetParserSettings(AllAttributesParserWrapperBase* ptr, const ParserSettings* parser_settings)
{
    ptr->get_parser()->set_parser_settings(*parser_settings);
}

void DisposeCharArray(char* data)
{
    if (data)
        delete[] data;
}

bool Parse(AllAttributesParserWrapperBase* ptr, int line, const char* data, char** message, bool single_line_msg)
{
    std::string msg;
    auto        ret = ptr->get_parser()->parse(line, data, msg, single_line_msg);
    if (!msg.empty())
    {
        *message = new char[msg.size() + 1];
        memcpy(*message, &msg[0], msg.size());
        (*message)[msg.size()] = 0;
    }
    return ret;
}

bool ParseValue(AllAttributesParserWrapperBase* ptr, int line, const char* data, WordValueWrapper** value, int* length,
                char** message, bool single_line_msg)
{
    return ptr->ParseValue(line, data, value, length, message, single_line_msg);
}

void DisposeWordValueWrapper(WordValueWrapper* data, int length)
{
    if (!data)
        return;

    for (int x = 0; x < length; ++x)
    {
        if (data[x].word)
            delete[] data[x].word;
        if (data[x].value)
            delete[] data[x].value;
    }
    delete[] data;
}

void GetMacroValues(AllAttributesParserWrapperBase* ptr, int** macro_ids, int** macro_lines, double** macro_values,
                    int* length)
{
    ptr->GetMacroValues(macro_ids, macro_lines, macro_values, length);
}

void GetLineMacroValues(AllAttributesParserWrapperBase* ptr, int macro_line, int** macro_ids, double** macro_values,
                        int* length)
{
    ptr->GetLineMacroValues(macro_line, macro_ids, macro_values, length);
}

void DisposeMacroValues(int* macro_ids, int* macro_lines, double* macro_values)
{
    if (macro_ids)
        delete[] macro_ids;
    if (macro_lines)
        delete[] macro_lines;
    if (macro_values)
        delete[] macro_values;
}

void ResetMacroValues(AllAttributesParserWrapperBase* ptr)
{
    ptr->ResetMacroValues();
}

void GetPathValues(AllAttributesParserWrapperBase* ptr, PathResultWrapper*& path_result,
                   TimeResultWrapper*& time_result, WordResultRange** word_result_range, int* length, int* unit_system)
{
    ptr->GetPathValues(path_result, time_result, word_result_range, length, unit_system);
}

void DisposePathValues(PathResultWrapper* path_result, TimeResultWrapper* time_result,
                       WordResultRange* word_result_range, int length)
{
    if (path_result)
    {
        if (path_result->tool_id)
            delete[] path_result->tool_id;
        delete path_result;
    }

    if (time_result)
    {
        if (time_result->tool_id)
            delete[] time_result->tool_id;
        delete time_result;
    }

    if (!word_result_range)
        return;
    for (int x = 0; x < length; ++x)
        if (word_result_range[x].word)
            delete[] word_result_range[x].word;
    delete[] word_result_range;
}

void ResetPathValues(AllAttributesParserWrapperBase* ptr)
{
    ptr->get_parser()->reset_attributes_path_calculator();
}

bool ConvertLength(AllAttributesParserWrapperBase* ptr, int line, const char* data, WordValueWrapper** value,
                   int* length, char** message, bool single_line_msg, UnitConversionType conversion_type)
{
    return ptr->ConvertLength(line, data, value, length, message, single_line_msg, conversion_type);
}

bool RemoveNumbering(AllAttributesParserWrapperBase* ptr, int line, const char* data, WordValueWrapper** value,
                     int* length, char** message, bool single_line_msg)
{
    return ptr->RemoveNumbering(line, data, value, length, message, single_line_msg);
}

bool Renumber(AllAttributesParserWrapperBase* ptr, int line, const char* data, WordValueWrapper** value, int* length,
              char** message, bool single_line_msg, RenumberSettingsWrapper* renumber_settings)
{
    RenumberSettings rs{};
    rs.word           = renumber_settings->word;
    rs.step           = renumber_settings->step;
    rs.frequency      = renumber_settings->frequency;
    rs.max_number     = renumber_settings->max_number;
    rs.existing_only  = renumber_settings->existing_only;
    rs.leading_zeroes = renumber_settings->leading_zeroes;

    return ptr->Renumber(line, data, value, length, message, single_line_msg, rs);
}

void SetRenumberStartValue(AllAttributesParserWrapperBase* ptr, unsigned int value)
{
    ptr->get_parser()->set_renumber_start_value(value);
}

void RenumberReset(AllAttributesParserWrapperBase* ptr)
{
    ptr->get_parser()->renumber_reset();
}

bool RotateAxes(AllAttributesParserWrapperBase* ptr, int line, const char* data, WordValueWrapper** value, int* length,
                char** message, bool single_line_msg, AxesRotatingOption axes_rotating_options)
{
    return ptr->RotateAxes(line, data, value, length, message, single_line_msg, axes_rotating_options);
}

Kinematics fill_kinematics(KinematicsWrapper* kinematics_wrapper, AxisParametersWrapper* kinematics_axis_parameters,
                           int kinematics_axis_parameters_length)
{
    Kinematics kinematics;

    kinematics.max_working_feed                  = kinematics_wrapper->max_working_feed;
    kinematics.max_fast_feed                     = kinematics_wrapper->max_fast_feed;
    kinematics.maximum_spindle_speed             = kinematics_wrapper->maximum_spindle_speed;
    kinematics.numer_of_items_in_the_warehouse   = kinematics_wrapper->numer_of_items_in_the_warehouse;
    kinematics.tool_exchange_time                = kinematics_wrapper->tool_exchange_time;
    kinematics.pallet_exchange_time              = kinematics_wrapper->pallet_exchange_time;
    kinematics.tool_measurement_time             = kinematics_wrapper->tool_measurement_time;
    kinematics.diameter_programming_2x           = kinematics_wrapper->diameter_programming_2x;
    kinematics.auto_measure_after_tool_selection = kinematics_wrapper->auto_measure_after_tool_selection;
    kinematics.pallet_exchange_code              = kinematics_wrapper->pallet_exchange_code;
    kinematics.pallet_exchange_code_value        = kinematics_wrapper->pallet_exchange_code_value;
    kinematics.tool_measurement_code             = kinematics_wrapper->tool_measurement_code;
    kinematics.tool_measurement_code_value       = kinematics_wrapper->tool_measurement_code_value;

    for (int x = 0; x < kinematics_axis_parameters_length; ++x)
    {
        const auto& kap                            = kinematics_axis_parameters[x];
        kinematics.cartesian_system_axis[kap.axis] = {kap.range_min, kap.range_max};
    }

    return kinematics;
}

void SetNCSettings(AllAttributesParserWrapperBase* ptr, EMachineToolType machine_tool_type,
                   char** machine_base_point_default_keys, double* machine_base_point_default_vals,
                   int machine_base_point_default_length, char** tool_exchange_point_default_keys,
                   double* tool_exchange_point_default_vals, int tool_exchange_point_default_length,
                   KinematicsWrapper* kinematics_wrapper, AxisParametersWrapper* kinematics_axis_parameters,
                   int kinematics_axis_parameters_length, CncDefaultValues* cnc_default_values, ZeroPoint* zero_point)
{
    ptr->get_parser()->set_ncsettings(
        machine_tool_type,
        makeMachinePointsData(machine_base_point_default_length, machine_base_point_default_keys,
                              machine_base_point_default_vals, tool_exchange_point_default_length,
                              tool_exchange_point_default_keys, tool_exchange_point_default_vals),
        fill_kinematics(kinematics_wrapper, kinematics_axis_parameters, kinematics_axis_parameters_length),
        std::move(*cnc_default_values), std::move(*zero_point));
}

bool SplitFile(AllAttributesParserWrapperBase* ptr, FileSplittingWrapper* fsw, const char** data, int data_length,
               WordValueWrapper** value,
               int**              value_value_length, // value[][][x]
               int**              value_length,       // value[][x][]
               int*               length,             // value[x][][]
               char** message, bool single_line_msg)
{
    return ptr->SplitFile(fsw, data, data_length, value, value_value_length, value_length, length, message,
                          single_line_msg);
}

void DisposeSplitFile(WordValueWrapper* value, int* value_value_length, int* value_length, int length)
{
    if (value)
    {
        int               cnt = 0;
        WordValueWrapper* ptr = value;
        for (int x = 0; x < length; ++x)
        {
            for (int y = 0; y < value_length[x]; ++y, ++cnt)
            {
                for (int z = 0; z < value_value_length[cnt]; ++z, ++ptr)
                {
                    if (ptr->word)
                        delete[] ptr->word;
                    if (ptr->value)
                        delete[] ptr->value;
                }
            }
        }
        delete[] value;
    }
    if (value_value_length)
        delete[] value_value_length;
    if (value_length)
        delete[] value_length;
}

bool SplitFileString(AllAttributesParserWrapperBase* ptr, FileSplittingWrapper* fsw, const char** data, int data_length,
                     char*** value,
                     int*    length, // value[x][]
                     char** message, bool single_line_msg)
{
    return ptr->SplitFileString(fsw, data, data_length, value, length, message, single_line_msg);
}

void DisposeSplitFileString(char** value, int length)
{
    if (!value)
        return;

    for (int x = 0; x < length; ++x)
    {
        if (value[x])
            delete[] value[x];
    }
    delete[] value;
}

bool RemoveComment(AllAttributesParserWrapperBase* ptr, int line, const char* data, WordValueWrapper** value,
                   int* length, char** message, bool single_line_msg)
{
    return ptr->RemoveComment(line, data, value, length, message, single_line_msg);
}

bool RemoveOptionalBlock(AllAttributesParserWrapperBase* ptr, int line, const char* data, WordValueWrapper** value,
                         int* length, char** message, bool single_line_msg)
{
    return ptr->RemoveOptionalBlock(line, data, value, length, message, single_line_msg);
}

} // namespace parser
