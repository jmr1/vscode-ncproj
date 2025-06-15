#pragma once

#include "parser_export.h"

#include <string>
#include <string_view>
#include <vector>

#include <boost/spirit/include/qi_symbols.hpp>

#include "AllAttributesParserBase.h"
#include "AllAttributesParserDefines.h"
#include "AllAttributesParserGrammar.h"
#include "AttributesPathCalculator.h"
#include "CodeGroupsDefines.h"
#include "MacroDefines.h"
#include "Renumberer.h"
#include "RotationVerifier.h"
#include "UnitConverter.h"

namespace qi = boost::spirit::qi;

using word_symbols = qi::symbols<char, std::string>;

namespace parser {
namespace fanuc {

struct FanucAttributeData : public AttributeVariantData
{
    std::vector<AttributeVariant> value;
};

class PARSER_API AllAttributesParser final : public AllAttributesParserBase
{
public:
    AllAttributesParser(FanucWordGrammar&& grammar, std::vector<std::string>&& operations, code_groups_map&& gcodes,
                        code_groups_map&& mcodes, ParserSettings&& parser_settings, OtherSettings&& other_settings,
                        EFanucParserType fanuc_parser_type, bool instantiateWithoutNCSettings = false);

    virtual ~AllAttributesParser()                             = default;
    AllAttributesParser(const AllAttributesParser&)            = delete;
    AllAttributesParser(AllAttributesParser&&)                 = default;
    AllAttributesParser& operator=(const AllAttributesParser&) = delete;
    AllAttributesParser& operator=(AllAttributesParser&&)      = default;

    virtual bool parse(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                       bool single_line_msg, const ParserSettings& parser_settings) override;
    virtual bool parse(int line, std::string_view data, std::string& message, bool single_line_msg) override;
    virtual bool parse(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                       bool single_line_msg) override;

    virtual bool simple_parse(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                              bool single_line_msg) override;

    virtual void set_parser_settings(const ParserSettings& parser_settings) override;

    virtual void set_ncsettings(EMachineTool machine_tool, EMachineToolType machine_tool_type,
                                MachinePointsData&& machine_points_data, Kinematics&& kinematics,
                                CncDefaultValues&& cnc_default_values, ZeroPoint&& zero_point) override;

    virtual EDriverUnits get_unit_system() const override;

    const macro_map& get_macro_values() const;
    void             reset_macro_values(bool init = true);

    virtual void              reset_attributes_path_calculator() override;
    virtual const PathResult& get_path_result() const override;
    virtual const TimeResult& get_time_result() const override;

    const word_range_map& get_word_range() const;

    virtual bool convert_length(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg, UnitConversionType conversion_type) override;

    virtual bool remove_numbering(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                                  bool single_line_msg) override;
    virtual bool renumber(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                          bool single_line_msg, const RenumberSettings& renumber_settings) override;
    virtual void set_renumber_start_value(unsigned int value) override;
    virtual void renumber_reset() override;

    virtual bool rotate_axes(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                             bool single_line_msg, AxesRotatingOption axes_rotating_options) override;

    bool split_file(std::vector<std::string>&& data, std::vector<std::vector<std::vector<AttributeVariant>>>& value,
                    std::string& message, bool single_line_msg, const FileSplitting& file_splitting);

    virtual bool remove_comment(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg) override;

    virtual bool remove_optional_block(int line, const std::string& data, AttributeVariantData& value,
                                       std::string& message, bool single_line_msg) override;

private:
    bool parse(int line, std::string_view data, std::vector<AttributeVariant>& value, std::string& message,
               bool single_line_msg, const ParserSettings& parser_settings);
    bool parse(int line, std::string_view data, std::vector<AttributeVariant>& value, std::string& message,
               bool single_line_msg);

    bool convert_length(int line, std::string_view data, std::vector<AttributeVariant>& value, std::string& message,
                        bool single_line_msg, UnitConversionType conversion_type);

    bool remove_numbering(int line, const std::string& data, std::vector<AttributeVariant>& value, std::string& message,
                          bool single_line_msg);
    bool renumber(int line, const std::string& data, std::vector<AttributeVariant>& value, std::string& message,
                  bool single_line_msg, const RenumberSettings& renumber_settings);

    bool rotate_axes(int line, std::string_view data, std::vector<AttributeVariant>& value, std::string& message,
                     bool single_line_msg, AxesRotatingOption axes_rotating_options);

    // bool split_file(std::vector<std::string> &&data, std::vector<std::vector<std::vector<AttributeVariant>>> &value,
    //    std::string &message, bool single_line_msg, const FileSplitting &file_splitting);

    bool remove_comment(int line, const std::string& data, std::vector<AttributeVariant>& value, std::string& message,
                        bool single_line_msg);

    bool remove_optional_block(int line, const std::string& data, std::vector<AttributeVariant>& value,
                               std::string& message, bool single_line_msg);

    void build_symbols();

    bool simple_parse(int line, std::string_view data, std::vector<AttributeVariant>& value, std::string& message,
                      bool single_line_msg);

    void init_macro_values();

private:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    FanucWordGrammar                                           word_grammar;
    const word_map*                                            active_word_grammar{};
    std::vector<std::string>                                   allowed_operations;
    code_groups_map                                            gcode_groups;
    code_groups_map                                            mcode_groups;
    ZeroPoint                                                  zero_point{};
    std::unique_ptr<AttributesPathCalculator>                  attr_path_calc;
    std::unique_ptr<all_attributes_grammar<pos_iterator_type>> all_attr_grammar;
    UnitConverter                                              unit_converter;
    Renumberer                                                 renumberer;
    RotationVerifier                                           rotation_verifier;
    macro_map                                                  macro_values;
    word_symbols                                               assignable_sym;
    word_symbols                                               decimal_sym;
    word_symbols                                               char_sym;
    ParserSettings                                             parser_settings{};
    OtherSettings                                              other_settings{};
    EMachineTool                                               machine_tool{};
    EMachineToolType                                           machine_tool_type{};
    EFanucParserType                                           fanuc_parser_type;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
};

} // namespace fanuc
} // namespace parser
