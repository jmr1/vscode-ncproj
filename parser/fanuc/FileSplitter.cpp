#include "stdafx.h"

#include "FileSplitter.h"

#include <optional>
#include <string>
#include <vector>

#include <boost/variant.hpp>

#include "MessageTextImpl.h"
#include "PathCalculator.h"

namespace parser {
namespace fanuc {

extern bool is_drill_cycle(int data);
extern bool is_drill_cycle_qparam(int data);

class FileSplitterAttributesVisitor : public boost::static_visitor<>
{
public:
    explicit FileSplitterAttributesVisitor(const CncDefaultValues& cnc_default_values)
        : cnc_default_values(cnc_default_values)
    {
    }

    void operator()(const DecimalAttributeData& data)
    {
        if (data.word == "/")
        {
            was_comment = true;
            return;
        }

        if (was_comment)
            return;

        if (data.word == "G")
        {
            if (!data.value)
                return;
            const auto val = std::stoi(*data.value);
            if ((val >= 0 && val <= 3))
            {
                active_g = val;
            }
            else if (is_drill_cycle(val) || is_drill_cycle_qparam(val))
            {
                active_g    = val;
                drill_cycle = true;
            }
            else if (val == 80 || (active_g == 0 && cnc_default_values.rapid_traverse_cancel_cycle) ||
                     (active_g == 1 && cnc_default_values.linear_interpolation_cancel_cycle))
            {
                active_g    = val;
                drill_cycle = false;
            }
            else
            {
                other_g = std::make_optional(val);
            }
        }
        else if (data.word == "M")
        {
            if (!data.value)
                return;
            if (data.dot || data.value2)
                return;
            if (!cnc_default_values.tool_number_executes_exchange && std::stoi(*data.value) == 6)
            {
                tool_change = true;
                ++tool_change_cnt;
            }
        }
        else if (data.word == "T")
        {
            if (!data.value)
                return;
            if (data.dot || data.value2)
                return;
            if (cnc_default_values.tool_number_executes_exchange)
            {
                tool_change = true;
                ++tool_change_cnt;
            }
        }
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

    void reset()
    {
        was_comment = false;
        tool_change = false;
        other_g     = std::nullopt;
        if (active_g > 3)
            active_g = -1;
    }

    auto g() const
    {
        return active_g;
    }

    auto is_drill_cycle_one_block_after_start() const
    {
        return (active_g == -1 && drill_cycle) || active_g == 80;
    }

    auto get_tool_change_cnt() const
    {
        return tool_change_cnt;
    }

    auto was_tool_changed() const
    {
        return tool_change;
    }

private:
    const CncDefaultValues& cnc_default_values;
    std::optional<int>      other_g{};
    int                     active_g{-1};
    bool                    was_comment{};
    bool                    drill_cycle{};
    bool                    tool_change{};
    int                     tool_change_cnt{};
};

std::vector<std::vector<std::string>> FileSplitter::evaluate(std::vector<std::string>&& data)
{
    std::vector<std::vector<std::string>> tmp(1);
    for (auto&& x : data)
    {
        if (x.find(file_splitting.text) == std::string::npos)
        {
            tmp.back().emplace_back(std::move(x));
        }
        else
        {
            if (!file_splitting.retraction_plane.empty())
                tmp.back().push_back(file_splitting.retraction_plane);
            tmp.emplace_back(std::vector<std::string>{});
            if (!file_splitting.retraction_plane.empty())
                tmp.back().push_back(file_splitting.retraction_plane);
            tmp.back().emplace_back(std::move(x));
        }
    }

    return tmp;
}

void FileSplitter::evaluate(const CncDefaultValues&                                  cnc_default_values,
                            std::vector<std::vector<AttributeVariant>>&&             data,
                            const std::vector<AttributeVariant>&                     retraction_plane,
                            std::vector<std::vector<std::vector<AttributeVariant>>>& value)
{
    if (file_splitting.type == FileSplittingType::block_numbers)
        evaluate_block_numbers(cnc_default_values, std::move(data), retraction_plane, value);
    else if (file_splitting.type == FileSplittingType::tool_change)
        evaluate_tool_change(cnc_default_values, std::move(data), retraction_plane, value);
}

void FileSplitter::evaluate_block_numbers(const CncDefaultValues&                                  cnc_default_values,
                                          std::vector<std::vector<AttributeVariant>>&&             data,
                                          const std::vector<AttributeVariant>&                     retraction_plane,
                                          std::vector<std::vector<std::vector<AttributeVariant>>>& value)
{
    if (file_splitting.step <= 0)
        throw file_splitter_exception(make_message(MessageName::CannotSplitFileNumberOfBlocks, language));

    int                           cnt{};
    FileSplitterAttributesVisitor av(cnc_default_values);
    value.resize(1);
    for (const auto& datax : data)
    {
        av.reset();
        for (const auto& datay : datax)
            boost::apply_visitor(av, datay);

        if (cnt < file_splitting.step || av.is_drill_cycle_one_block_after_start() ||
            (file_splitting.rapid_traverse_only && av.g() != 0))
        {
            value.back().emplace_back(std::move(datax));
        }
        else
        {
            cnt = 0;
            if (!file_splitting.retraction_plane.empty())
                value.back().push_back(retraction_plane);
            value.emplace_back(std::vector<std::vector<AttributeVariant>>{});
            if (!file_splitting.retraction_plane.empty())
                value.back().push_back(retraction_plane);
            value.back().emplace_back(std::move(datax));
        }

        ++cnt;
    }
}

void FileSplitter::evaluate_tool_change(const CncDefaultValues&                                  cnc_default_values,
                                        std::vector<std::vector<AttributeVariant>>&&             data,
                                        const std::vector<AttributeVariant>&                     retraction_plane,
                                        std::vector<std::vector<std::vector<AttributeVariant>>>& value)
{
    FileSplitterAttributesVisitor av(cnc_default_values);
    value.resize(1);
    for (const auto& datax : data)
    {
        av.reset();
        for (const auto& datay : datax)
            boost::apply_visitor(av, datay);

        if (!(av.was_tool_changed() && av.get_tool_change_cnt() > 1))
        {
            value.back().emplace_back(std::move(datax));
        }
        else
        {
            if (!file_splitting.retraction_plane.empty())
                value.back().push_back(retraction_plane);
            value.emplace_back(std::vector<std::vector<AttributeVariant>>{});
            if (!file_splitting.retraction_plane.empty())
                value.back().push_back(retraction_plane);
            value.back().emplace_back(std::move(datax));
        }
    }
}

void FileSplitter::evaluate(const CncDefaultValues&                                  cnc_default_values,
                            std::vector<std::vector<AttributeVariant>>&&             data,
                            const std::vector<AttributeVariant>&                     retraction_plane,
                            std::vector<std::vector<std::vector<AttributeVariant>>>& value,
                            const std::vector<PathResult>&                           vec_path_result)
{
    if (file_splitting.path <= 0)
        throw file_splitter_exception(make_message(MessageName::CannotSplitFileToolPathLength, language));

    int                           cnt{};
    double                        path{};
    FileSplitterAttributesVisitor av(cnc_default_values);
    value.resize(1);
    for (const auto& datax : data)
    {
        av.reset();
        for (const auto& datay : datax)
            boost::apply_visitor(av, datay);

        if (path < file_splitting.path || av.is_drill_cycle_one_block_after_start() ||
            (file_splitting.rapid_traverse_only && av.g() != 0))
        {
            value.back().emplace_back(std::move(datax));
        }
        else
        {
            path = 0;
            if (!file_splitting.retraction_plane.empty())
                value.back().push_back(retraction_plane);
            value.emplace_back(std::vector<std::vector<AttributeVariant>>{});
            if (!file_splitting.retraction_plane.empty())
                value.back().push_back(retraction_plane);
            value.back().emplace_back(std::move(datax));
        }

        path += vec_path_result[cnt].work_motion;
        ++cnt;
    }
}

void FileSplitter::evaluate(const CncDefaultValues&                                  cnc_default_values,
                            std::vector<std::vector<AttributeVariant>>&&             data,
                            const std::vector<AttributeVariant>&                     retraction_plane,
                            std::vector<std::vector<std::vector<AttributeVariant>>>& value,
                            const std::vector<TimeResult>&                           vec_time_result)
{
    if (file_splitting.time <= 0)
        throw file_splitter_exception(make_message(MessageName::CannotSplitFileToolTime, language));

    int                           cnt{};
    double                        time{};
    FileSplitterAttributesVisitor av(cnc_default_values);
    value.resize(1);
    for (const auto& datax : data)
    {
        av.reset();
        for (const auto& datay : datax)
            boost::apply_visitor(av, datay);

        if (time < file_splitting.time || av.is_drill_cycle_one_block_after_start() ||
            (file_splitting.rapid_traverse_only && av.g() != 0))
        {
            value.back().emplace_back(std::move(datax));
        }
        else
        {
            time = 0;
            if (!file_splitting.retraction_plane.empty())
                value.back().push_back(retraction_plane);
            value.emplace_back(std::vector<std::vector<AttributeVariant>>{});
            if (!file_splitting.retraction_plane.empty())
                value.back().push_back(retraction_plane);
            value.back().emplace_back(std::move(datax));
        }

        time += vec_time_result[cnt].work_motion;
        ++cnt;
    }
}

} // namespace fanuc
} // namespace parser
