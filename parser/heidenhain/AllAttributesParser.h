#pragma once

#include "parser_export.h"

#include <string>
#include <string_view>
#include <vector>

#include <boost/spirit/include/qi_symbols.hpp>

#include "AllAttributesParserBase.h"
#include "AllAttributesParserDefines.h"
#include "AllAttributesParserGrammar.h"
#include "GeneralParserDefines.h"

namespace qi = boost::spirit::qi;

using word_symbols = qi::symbols<char, std::string>;

namespace parser {
namespace heidenhain {

struct HeidenhainAttributeData : public AttributeVariantData
{
    std::vector<AttributeVariant> value;
};

class PARSER_API AllAttributesParser final : public AllAttributesParserBase
{
public:
    AllAttributesParser(ParserSettings&& parser_settings, OtherSettings&& other_settings,
                        bool instantiateWithoutNCSettings = false);

    virtual ~AllAttributesParser()                             = default;
    AllAttributesParser(const AllAttributesParser&)            = delete;
    AllAttributesParser(AllAttributesParser&&)                 = default;
    AllAttributesParser& operator=(const AllAttributesParser&) = delete;
    AllAttributesParser& operator=(AllAttributesParser&&)      = default;

    virtual bool parse(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                       bool single_line_msg, const ParserSettings& parser_settings) override;
    virtual bool parse(int line, std::string_view data, AttributeVariantData& attr, std::string& message,
                       bool single_line_msg) override;
    virtual bool parse(int line, std::string_view data, std::string& message, bool single_line_msg) override;

    virtual void set_parser_settings(const ParserSettings& parser_settings) override;

    virtual void set_ncsettings(EMachineTool machine_tool, EMachineToolType machine_tool_type,
                                MachinePointsData&& machine_points_data, Kinematics&& kinematics,
                                CncDefaultValues&& cnc_default_values, ZeroPoint&& zero_point) override;

    virtual EDriverUnits get_unit_system() const override
    {
        throw(0);
    }

    // virtual const macro_map& get_macro_values() const override;
    // virtual void reset_macro_values() override;

    virtual void reset_attributes_path_calculator() override
    {
    }
    virtual const PathResult& get_path_result() const override
    {
        throw(0);
    }
    virtual const TimeResult& get_time_result() const override
    {
        throw(0);
    }

    // virtual const word_range_map& get_word_range() const override;

    virtual bool convert_length(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg, UnitConversionType conversion_type) override
    {
        return false;
    }

    virtual bool remove_numbering(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                                  bool single_line_msg) override
    {
        return false;
    }
    virtual bool renumber(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                          bool single_line_msg, const RenumberSettings& renumber_settings) override
    {
        return false;
    }
    virtual void set_renumber_start_value(unsigned int value) override
    {
    }
    virtual void renumber_reset() override
    {
    }

    virtual bool rotate_axes(int line, std::string_view data, AttributeVariantData& value, std::string& message,
                             bool single_line_msg, AxesRotatingOption axes_rotating_options) override
    {
        return false;
    }

    virtual bool split_file(std::vector<std::string>&&                               data,
                            std::vector<std::vector<std::vector<AttributeVariant>>>& value, std::string& message,
                            bool single_line_msg, const FileSplitting& file_splitting)
    {
        return false;
    }

    virtual bool remove_comment(int line, const std::string& data, AttributeVariantData& value, std::string& message,
                                bool single_line_msg) override
    {
        return false;
    }

    virtual bool remove_optional_block(int line, const std::string& data, AttributeVariantData& value,
                                       std::string& message, bool single_line_msg) override
    {
        return false;
    }

private:
    bool parse(int line, std::string_view data, std::vector<AttributeVariant>& value, std::string& message,
               bool single_line_msg, const ParserSettings& parser_settings);
    bool parse(int line, std::string_view data, std::vector<AttributeVariant>& attr, std::string& message,
               bool single_line_msg);

    void build_symbols();

    bool simple_parse(int line, const std::string& data, std::vector<AttributeVariant>& value, std::string& message,
                      bool single_line_msg);

private:
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
    std::vector<std::string> allowed_operations{
        "K1X", "K1Y", "K1Z", "K2X", "K2Y", "K2Z", "K3X", "K3Y", "K3Z",
        "P01", "P02", "P03", "P04", "P05", "P06", "P07", "P1X", "P1Y",
    };
    ZeroPoint                                                  zero_point{};
    word_symbols                                               sym;
    ParserSettings                                             parser_settings{};
    OtherSettings                                              other_settings{};
    std::unique_ptr<all_attributes_grammar<pos_iterator_type>> all_attr_grammar;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
};

} // namespace heidenhain
} // namespace parser
