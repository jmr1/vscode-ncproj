#include "stdafx.h"

#include "AxesRotator.h"

#include <string>
#include <unordered_map>

#include <boost/functional/hash.hpp>
#include <boost/variant.hpp>

#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

struct WordTransform
{
    std::string word;
    int         value;
};

struct WordTransformHasher
{
    size_t operator()(const AttributeData<std::string>& data) const
    {
        size_t seed = 0;

        boost::hash_combine(seed, boost::hash_value(data.word));
        boost::hash_combine(seed, boost::hash_value(data.value));

        return seed;
    }
};

using WordTransformMap = std::unordered_map<AttributeData<std::string>, WordTransform, WordTransformHasher>;

using AxesRotatingOptionsMap = std::unordered_map<AxesRotatingOption, WordTransformMap>;

const AxesRotatingOptionsMap wtom = {
    {AxesRotatingOption::Xrotate90degrees,
     {
         {{"X", ""}, {"X", 1}},
         {{"Y", ""}, {"Z", -1}},
         {{"Z", ""}, {"Y", 1}},
         {{"I", ""}, {"I", 1}},
         {{"J", ""}, {"K", -1}},
         {{"K", ""}, {"J", 1}},
         {{"G", "17"}, {"G", 18}},
         {{"G", "18"}, {"G", 17}},
         {{"G", "19"}, {"G", 19}},
         {{"G", "2"}, {"G", 2}},
         {{"G", "3"}, {"G", 3}},
     }},
    {AxesRotatingOption::Xrotate180degrees,
     {
         {{"X", ""}, {"X", 1}},
         {{"Y", ""}, {"Y", -1}},
         {{"Z", ""}, {"Z", -1}},
         {{"I", ""}, {"I", 1}},
         {{"J", ""}, {"J", -1}},
         {{"K", ""}, {"K", -1}},
         {{"G", "17"}, {"G", 17}},
         {{"G", "18"}, {"G", 18}},
         {{"G", "19"}, {"G", 19}},
         {{"G", "2"}, {"G", 3}},
         {{"G", "3"}, {"G", 2}},
     }},
    {AxesRotatingOption::Xrotate270degrees,
     {
         {{"X", ""}, {"X", 1}},
         {{"Y", ""}, {"Z", 1}},
         {{"Z", ""}, {"Y", -1}},
         {{"I", ""}, {"I", 1}},
         {{"J", ""}, {"K", 1}},
         {{"K", ""}, {"J", -1}},
         {{"G", "17"}, {"G", 18}},
         {{"G", "18"}, {"G", 17}},
         {{"G", "19"}, {"G", 19}},
         {{"G", "2"}, {"G", 3}},
         {{"G", "3"}, {"G", 2}},
     }},
    {AxesRotatingOption::Yrotate90degrees,
     {
         {{"X", ""}, {"Z", 1}},
         {{"Y", ""}, {"Y", 1}},
         {{"Z", ""}, {"X", -1}},
         {{"I", ""}, {"K", 1}},
         {{"J", ""}, {"J", 1}},
         {{"K", ""}, {"I", -1}},
         {{"G", "17"}, {"G", 19}},
         {{"G", "18"}, {"G", 18}},
         {{"G", "19"}, {"G", 17}},
         {{"G", "2"}, {"G", 3}},
         {{"G", "3"}, {"G", 2}},
     }},
    {AxesRotatingOption::Yrotate180degrees,
     {
         {{"X", ""}, {"X", -1}},
         {{"Y", ""}, {"Y", 1}},
         {{"Z", ""}, {"Z", -1}},
         {{"I", ""}, {"I", -1}},
         {{"J", ""}, {"J", 1}},
         {{"K", ""}, {"K", -1}},
         {{"G", "17"}, {"G", 17}},
         {{"G", "18"}, {"G", 18}},
         {{"G", "19"}, {"G", 19}},
         {{"G", "2"}, {"G", 3}},
         {{"G", "3"}, {"G", 2}},
     }},
    {AxesRotatingOption::Yrotate270degrees,
     {
         {{"X", ""}, {"Z", -1}},
         {{"Y", ""}, {"Y", 1}},
         {{"Z", ""}, {"X", 1}},
         {{"I", ""}, {"K", -1}},
         {{"J", ""}, {"J", 1}},
         {{"K", ""}, {"I", 1}},
         {{"G", "17"}, {"G", 19}},
         {{"G", "18"}, {"G", 18}},
         {{"G", "19"}, {"G", 17}},
         {{"G", "2"}, {"G", 2}},
         {{"G", "3"}, {"G", 3}},
     }},
    {AxesRotatingOption::Zrotate90degrees,
     {
         {{"X", ""}, {"Y", -1}},
         {{"Y", ""}, {"X", 1}},
         {{"Z", ""}, {"Z", 1}},
         {{"I", ""}, {"J", -1}},
         {{"J", ""}, {"I", 1}},
         {{"K", ""}, {"K", 1}},
         {{"G", "17"}, {"G", 17}},
         {{"G", "18"}, {"G", 19}},
         {{"G", "19"}, {"G", 18}},
         {{"G", "2"}, {"G", 2}},
         {{"G", "3"}, {"G", 3}},
     }},
    {AxesRotatingOption::Zrotate180degrees,
     {
         {{"X", ""}, {"X", -1}},
         {{"Y", ""}, {"Y", -1}},
         {{"Z", ""}, {"Z", 1}},
         {{"I", ""}, {"I", -1}},
         {{"J", ""}, {"J", -1}},
         {{"K", ""}, {"K", 1}},
         {{"G", "17"}, {"G", 17}},
         {{"G", "18"}, {"G", 18}},
         {{"G", "19"}, {"G", 19}},
         {{"G", "2"}, {"G", 2}},
         {{"G", "3"}, {"G", 3}},
     }},
    {AxesRotatingOption::Zrotate270degrees,
     {
         {{"X", ""}, {"Y", 1}},
         {{"Y", ""}, {"X", -1}},
         {{"Z", ""}, {"Z", 1}},
         {{"I", ""}, {"J", 1}},
         {{"J", ""}, {"I", -1}},
         {{"K", ""}, {"K", 1}},
         {{"G", "17"}, {"G", 17}},
         {{"G", "18"}, {"G", 19}},
         {{"G", "19"}, {"G", 18}},
         {{"G", "2"}, {"G", 2}},
         {{"G", "3"}, {"G", 3}},
     }},
};

class AttributesAxesRotatorVisitor : public boost::static_visitor<>
{
public:
    explicit AttributesAxesRotatorVisitor(const WordTransformMap& wtm)
        : wtm(wtm)
    {
    }

    void operator()(DecimalAttributeData& data)
    {
        if (data.word == "X" || data.word == "Y" || data.word == "Z" || data.word == "I" || data.word == "J" ||
            data.word == "K")
        {
            const auto wt_item = find_if(std::begin(wtm), std::end(wtm),
                                         [&data](const auto& value) { return value.first.word == data.word; });
            data.word          = wt_item->second.word;
            if (wt_item->second.value == -1)
            {
                if (data.sign && *data.sign == '-')
                    data.sign = boost::none;
                else
                    data.sign = boost::make_optional('-');
            }
        }
        else if (data.word == "G")
        {
            if (!data.value)
                return;

            auto& val = *data.value;
            if (!(val == "17" || val == "18" || val == "19" || val == "2" || val == "3"))
                return;

            const auto wt_item = wtm.find({data.word, val});
            if (wt_item == std::end(wtm))
                return;

            val = std::to_string(wt_item->second.value);
        }
    }

    template <typename T>
    void operator()(const T& data)
    {
    }

private:
    const WordTransformMap& wtm;
};

void AxesRotator::operator()(AxesRotatingOption option, std::vector<AttributeVariant>& value)
{
    AttributesAxesRotatorVisitor attributes_visitor(wtom.at(option));
    for (size_t x = 0; x < value.size(); ++x)
        boost::apply_visitor(attributes_visitor, value[x]);
}

} // namespace fanuc
} // namespace parser
