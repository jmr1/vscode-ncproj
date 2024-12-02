#include "stdafx.h"

#include <boost/variant.hpp>

#include "AllAttributesParser.h"
#include "AllAttributesParserWrapper.h"

namespace parser {
namespace heidenhain {

AllAttributesParserWrapperBase* CreateHeidenhainAllAttributesParser(const ParserSettings* parser_settings,
                                                                    const OtherSettings*  other_settings,
                                                                    bool instantiateWithoutNCSettings /*= false*/)
{
    auto parser = std::make_unique<AllAttributesParser>(ParserSettings{
        parser_settings->evaluate_macro, parser_settings->verify_code_groups, parser_settings->calculate_path, parser_settings->ncsettings_code_analysis,
                       parser_settings->zero_point_analysis},
        OtherSettings{other_settings->language}, instantiateWithoutNCSettings);

    return new AllAttributesParserWrapper(std::move(parser));
}

/*class Attributes2WordValueWrapperVisitor : public boost::static_visitor<>
{
public:
    Attributes2WordValueWrapperVisitor(WordValueWrapper &wordValueWrapper)
        : wordValue(wordValueWrapper)
    {}

    void operator()(const DecimalAttributeData &data) const
    {
        add_word(data.word);
        std::string tmp;
        if(data.assign)
            tmp += *data.assign;
        if(data.sign)
            tmp += *data.sign;
        if(data.open_bracket)
            tmp += *data.open_bracket;
        if(data.macro)
            tmp += *data.macro;
        if(data.value)
            tmp += *data.value;
        if(data.dot)
            tmp += *data.dot;
        if(data.value2)
            tmp += *data.value2;
        if(data.close_bracket)
            tmp += *data.close_bracket;

        wordValue.value = new char[tmp.size()+1];
        if(!tmp.empty())
            memcpy(wordValue.value, &tmp[0], tmp.size());
        wordValue.value[tmp.size()] = 0;
    }

    void operator()(const StringAttributeData &data) const
    {
        add_word(data.word);
        auto size = data.value.size() + 1;
        if (data.word == "(")
            size += 1;
        wordValue.value = new char[size];
        if(!data.value.empty())
            memcpy(wordValue.value, &data.value[0], data.value.size());
        if (data.word == "(")
            wordValue.value[size - 2] = ')';
        wordValue.value[size-1] = 0;
    }

    void operator()(const CharAttributeData &data) const
    {
        add_word(data.word);
        wordValue.value = new char[2];
        wordValue.value[0] = data.value;
        wordValue.value[1] = 0;
    }

private:
    void add_word(const std::string &word) const
    {
        wordValue.word = new char[word.size()+1];
        if(!word.empty())
            memcpy(wordValue.word, &word[0], word.size());
        wordValue.word[word.size()] = 0;
    }

private:
    WordValueWrapper &wordValue;
};*/

void fill_message(const std::string& msg, char** message)
{
    if (!msg.empty())
    {
        *message = new char[msg.size() + 1];
        memcpy(*message, &msg[0], msg.size());
        (*message)[msg.size()] = 0;
    }
}

void fill_WordValueWrapper(const std::vector<AttributeVariant>& v, WordValueWrapper** value, int* length)
{
    /*if(!v.empty())
    {
        *value = new WordValueWrapper[v.size()];
        for(size_t x = 0; x < v.size(); ++x)
            boost::apply_visitor(Attributes2WordValueWrapperVisitor((*value)[x]), v[x]);
        *length = static_cast<int>(v.size());
    }*/
}

void fill_parsed_values(const std::string& msg, const std::vector<AttributeVariant>& v, WordValueWrapper** value,
                        int* length, char** message)
{
    fill_WordValueWrapper(v, value, length);
    fill_message(msg, message);
}

AllAttributesParserWrapper::AllAttributesParserWrapper(std::unique_ptr<AllAttributesParserBase> ptr)
    : AllAttributesParserWrapperBase(std::move(ptr))
{
}

bool AllAttributesParserWrapper::ParseValue(int line, const char* data, WordValueWrapper** value, int* length,
                                            char** message, bool single_line_msg)
{
    std::string             msg;
    HeidenhainAttributeData v;
    auto ret = dynamic_cast<AllAttributesParser*>(parser.get())->parse(line, data, v, msg, single_line_msg);
    fill_parsed_values(msg, v.value, value, length, message);

    return ret;
}

void AllAttributesParserWrapper::GetMacroValues(int** macro_ids, int** macro_lines, double** macro_values, int* length)
{
    *length = 0;
    /*const auto &mv = dynamic_cast<AllAttributesParser*>(parser.get())->get_macro_values();
    *length = static_cast<int>(mv.size());
    if (mv.empty())
        return;

    *macro_ids = new int[mv.size()];
    *macro_lines = new int[mv.size()];
    *macro_values = new double[mv.size()];

    int x = 0;
    for(const auto &item : mv)
    {
        (*macro_ids)[x] = item.first.id;
        (*macro_lines)[x] = item.first.line;
        (*macro_values)[x] = item.second;
        ++x;
    }*/
}

void AllAttributesParserWrapper::GetLineMacroValues(int macro_line, int** macro_ids, double** macro_values, int* length)
{
    *length = 0;
    /*const auto &mv = dynamic_cast<AllAttributesParser*>(parser.get())->get_macro_values();
    *length = 0;
    if (mv.empty())
        return;

    std::decay_t<decltype(mv)> line_mv;
    auto finder = [&mv, &macro_line](auto it){
        return std::find_if(it, std::cend(mv), [&macro_line](const auto &p){
            return p.first.line == macro_line;
        });
    };
    for(auto it = finder(std::cbegin(mv)); it != std::cend(mv); it = finder(++it))
        line_mv.insert(*it);

    if (line_mv.empty())
        return;

    *length = static_cast<int>(line_mv.size());
    *macro_ids = new int[mv.size()];
    *macro_values = new double[mv.size()];

    int x = 0;
    for(const auto &item : line_mv)
    {
        (*macro_ids)[x] = item.first.id;
        (*macro_values)[x] = item.second;
        ++x;
    }*/
}

void AllAttributesParserWrapper::ResetMacroValues()
{
    // dynamic_cast<AllAttributesParser*>(parser.get())->reset_macro_values();
}

template <typename T, typename S>
void convert(const T& value, S& prw)
{
    prw.total       = value.total;
    prw.work_motion = value.work_motion;
    prw.fast_motion = value.fast_motion;
    prw.tool_total  = value.tool_total;

    size_t size = value.tool_id.size() + 1;
    prw.tool_id = new char[size];
    if (!value.tool_id.empty())
        memcpy(prw.tool_id, &value.tool_id[0], value.tool_id.size());
    prw.tool_id[size - 1] = 0;
}

void AllAttributesParserWrapper::GetPathValues(PathResultWrapper*& path_result, TimeResultWrapper*& time_result,
                                               WordResultRange** word_result_range, int* length, int* unit_system)
{
    path_result = new PathResultWrapper{};
    convert(parser->get_path_result(), *path_result);

    time_result = new TimeResultWrapper{};
    convert(parser->get_time_result(), *time_result);

    /*const auto &word_range_map = dynamic_cast<AllAttributesParser*>(parser.get())->get_word_range();

    int cnt = 0;
    *word_result_range = new WordResultRange[word_range_map.size()];
    for(const auto &item : word_range_map)
    {
        (*word_result_range)[cnt].word = new char[item.first.size() + 1];
        memcpy((*word_result_range)[cnt].word, item.first.c_str(), item.first.size());
        (*word_result_range)[cnt].word[item.first.size()] = 0;
        const auto&[min, current, max] = item.second;
        (*word_result_range)[cnt].value_min = min;
        (*word_result_range)[cnt].value = current;
        (*word_result_range)[cnt].value_max = max;
        ++cnt;
    }

    *length = static_cast<int>(word_range_map.size());*/
    *length      = 0;
    *unit_system = static_cast<int>(parser->get_unit_system());
}

bool AllAttributesParserWrapper::ConvertLength(int line, const char* data, WordValueWrapper** value, int* length,
                                               char** message, bool single_line_msg, UnitConversionType conversion_type)
{
    std::string             msg;
    HeidenhainAttributeData v;
    auto                    ret = parser->convert_length(line, data, v, msg, single_line_msg, conversion_type);
    fill_parsed_values(msg, v.value, value, length, message);

    return ret;
}

bool AllAttributesParserWrapper::RemoveNumbering(int line, const char* data, WordValueWrapper** value, int* length,
                                                 char** message, bool single_line_msg)
{
    std::string             msg;
    HeidenhainAttributeData v;
    auto                    ret = parser->remove_numbering(line, data, v, msg, single_line_msg);
    fill_parsed_values(msg, v.value, value, length, message);

    return ret;
}

bool AllAttributesParserWrapper::Renumber(int line, const char* data, WordValueWrapper** value, int* length,
                                          char** message, bool single_line_msg, const RenumberSettings& rs)
{
    std::string             msg;
    HeidenhainAttributeData v;

    auto ret = parser->renumber(line, data, v, msg, single_line_msg, rs);
    fill_parsed_values(msg, v.value, value, length, message);

    return ret;
}

bool AllAttributesParserWrapper::RotateAxes(int line, const char* data, WordValueWrapper** value, int* length,
                                            char** message, bool single_line_msg,
                                            AxesRotatingOption axes_rotating_options)
{
    std::string             msg;
    HeidenhainAttributeData v;
    auto                    ret = parser->rotate_axes(line, data, v, msg, single_line_msg, axes_rotating_options);
    fill_parsed_values(msg, v.value, value, length, message);

    return ret;
}

bool AllAttributesParserWrapper::SplitFile(FileSplittingWrapper* fsw, const char** data, int data_length,
                                           WordValueWrapper** value,
                                           int**              value_value_length, // value[][][x]
                                           int**              value_length,       // value[][x][]
                                           int*               length,             // value[x][][]
                                           char** message, bool single_line_msg)
{
    std::string              msg;
    std::vector<std::string> str_data(data_length);
    for (int x = 0; x < data_length; ++x)
        str_data[x] = data[x];
    std::vector<std::vector<std::vector<AttributeVariant>>> ret_value;
    FileSplitting                                           file_splitting{
        fsw->type, fsw->step, fsw->time, fsw->path, fsw->text, fsw->rapid_traverse_only, fsw->retraction_plane};

    auto ret = dynamic_cast<AllAttributesParser*>(parser.get())
                   ->split_file(std::move(str_data), ret_value, msg, single_line_msg, file_splitting);

    size_t cnt{};
    size_t cnt_value_length{};
    for (size_t x = 0; x < ret_value.size(); ++x)
    {
        cnt_value_length += ret_value[x].size();
        for (size_t y = 0; y < ret_value[x].size(); ++y)
            cnt += ret_value[x][y].size();
    }

    *value              = new WordValueWrapper[cnt];
    *length             = static_cast<int>(ret_value.size());
    *value_length       = new int[ret_value.size()];
    *value_value_length = new int[cnt_value_length];

    cnt = 0;
    /*int cnt2{};
    for(size_t x = 0; x < ret_value.size(); ++x)
    {
        (*value_length)[x] = static_cast<int>(ret_value[x].size());
        for(size_t y = 0; y < ret_value[x].size(); ++y, ++cnt2)
        {
            (*value_value_length)[cnt2] = static_cast<int>(ret_value[x][y].size());
            for(size_t z = 0; z < ret_value[x][y].size(); ++z, ++cnt)
                boost::apply_visitor(Attributes2WordValueWrapperVisitor((*value)[cnt]), ret_value[x][y][z]);
        }
    }*/
    fill_message(msg, message);

    return ret;
}

/*class Attributes2StringVisitor : public boost::static_visitor<>
{
public:
    Attributes2StringVisitor(std::string &value)
        : value(value)
    {}

    void operator()(const DecimalAttributeData &data) const
    {
        value += data.word;
        if(data.assign)
            value += *data.assign;
        if(data.sign)
            value += *data.sign;
        if(data.open_bracket)
            value += *data.open_bracket;
        if(data.macro)
            value += *data.macro;
        if(data.value)
            value += *data.value;
        if(data.dot)
            value += *data.dot;
        if(data.value2)
            value += *data.value2;
        if(data.close_bracket)
            value += *data.close_bracket;
    }

    void operator()(const StringAttributeData &data) const
    {
        value += data.word;
        if(!data.value.empty())
            value += data.value;
        if (data.word == "(")
            value += ')';
    }

    void operator()(const CharAttributeData &data) const
    {
        value += data.word;
        if(data.value)
            value += data.value;
    }

private:
    std::string &value;
};*/

bool AllAttributesParserWrapper::SplitFileString(FileSplittingWrapper* fsw, const char** data, int data_length,
                                                 char*** value,
                                                 int*    length, // value[x][]
                                                 char** message, bool single_line_msg)
{
    std::string              msg;
    std::vector<std::string> str_data(data_length);
    for (int x = 0; x < data_length; ++x)
        str_data[x] = data[x];
    std::vector<std::vector<std::vector<AttributeVariant>>> ret_value;
    FileSplitting                                           file_splitting{
        fsw->type, fsw->step, fsw->time, fsw->path, fsw->text, fsw->rapid_traverse_only, fsw->retraction_plane};

    auto ret = dynamic_cast<AllAttributesParser*>(parser.get())
                   ->split_file(std::move(str_data), ret_value, msg, single_line_msg, file_splitting);

    *value  = new char*[ret_value.size()];
    *length = static_cast<int>(ret_value.size());

    for (size_t x = 0; x < ret_value.size(); ++x)
    {
        std::string tmp;
        for (size_t y = 0; y < ret_value[x].size(); ++y)
        {
            for (size_t z = 0; z < ret_value[x][y].size(); ++z)
            {
                if (z > 0)
                    tmp += " ";
                //                boost::apply_visitor(Attributes2StringVisitor(tmp), ret_value[x][y][z]);
            }
            tmp += "\r\n";
        }

        fill_message(tmp, &(*value)[x]);
    }

    fill_message(msg, message);

    return ret;
}

bool AllAttributesParserWrapper::RemoveComment(int line, const char* data, WordValueWrapper** value, int* length,
                                               char** message, bool single_line_msg)
{
    std::string             msg;
    HeidenhainAttributeData v;

    auto ret = parser->remove_comment(line, data, v, msg, single_line_msg);
    fill_parsed_values(msg, v.value, value, length, message);

    return ret;
}

bool AllAttributesParserWrapper::RemoveOptionalBlock(int line, const char* data, WordValueWrapper** value, int* length,
                                                     char** message, bool single_line_msg)
{
    std::string             msg;
    HeidenhainAttributeData v;

    auto ret = parser->remove_optional_block(line, data, v, msg, single_line_msg);
    fill_parsed_values(msg, v.value, value, length, message);

    return ret;
}

} // namespace heidenhain
} // namespace parser
