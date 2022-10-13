#include "stdafx.h"

#include "Renumberer.h"

#include <algorithm>
#include <boost/variant.hpp>

#include "GeneralParserDefines.h"
#include "util.h"

namespace parser {
namespace fanuc {

Renumberer::Renumberer(const FanucWordGrammar& word_grammar)
    : word_grammar(word_grammar)
{
    std::for_each(std::begin(word_grammar.metric), std::end(word_grammar.metric), [&](const auto& wg) {
        if (wg.second.word_type == WordType::numbering)
        {
            conv_word_grammar[wg.first] = &wg.second;
            conv_words.push_back(wg.first);
        }
    });
}

void Renumberer::remove(std::vector<AttributeVariant>& value)
{
    if (value.empty())
        return;
    const auto& attr     = value.front();
    const auto* attr_dec = boost::get<DecimalAttributeData>(&attr);
    if (!attr_dec)
        return;

    auto itor = std::find_if(std::cbegin(conv_words), std::cend(conv_words),
                             [&](const std::string& value) { return attr_dec->word == value; });
    if (itor != std::end(conv_words))
        value.erase(value.begin());
}

void Renumberer::renumber(std::vector<AttributeVariant>& value, const RenumberSettings& renumber_settings)
{
    if (value.empty())
        return;

    bool modify_existing_one{};
    bool bump_counter_up{};
    int  leading_zeroes_num{};
    if (renumber_settings.leading_zeroes)
        if (auto it = conv_word_grammar.find(renumber_settings.word); it != std::end(conv_word_grammar))
            leading_zeroes_num = static_cast<int>(std::to_string(it->second->range_to).size());

    auto& attr     = value.front();
    auto* attr_dec = boost::get<DecimalAttributeData>(&attr);
    if (attr_dec)
    {
        if (attr_dec->word == "/")
            return;

        auto itor = std::find_if(std::cbegin(conv_words), std::cend(conv_words),
                                 [&](const std::string& value) { return attr_dec->word == value; });
        if (itor != std::end(conv_words))
        {
            modify_existing_one = true;
            if (renumber_settings.frequency < 2 || (renumber_settings.frequency > 1 && current_frequency == 0))
            {
                attr_dec->word = renumber_settings.word;
                if (renumber_settings.leading_zeroes && leading_zeroes_num > 0)
                    attr_dec->value = to_string_leading_zeroes(counter, leading_zeroes_num);
                else
                    attr_dec->value = std::to_string(counter);
                bump_counter_up = true;
            }
            else
            {
                value.erase(value.begin());
            }
        }
        else
        {
            if (renumber_settings.existing_only)
                return; // no existing attribute
        }
    }
    else
    {
        if (renumber_settings.existing_only)
            return; // no existing attribute
        if (const auto* attr_c = boost::get<AttributeData<char>>(&attr))
            if (attr_c->word == "%")
                return;
        if (const auto* attr_s = boost::get<AttributeData<std::string>>(&attr))
            if (attr_s->word == "O" || attr_s->word == "(")
                return;
    }

    if (!modify_existing_one &&
        (renumber_settings.frequency < 2 || (renumber_settings.frequency > 1 && current_frequency == 0)))
    {
        DecimalAttributeData dad{};
        dad.word = renumber_settings.word;
        if (renumber_settings.leading_zeroes && leading_zeroes_num > 0)
            dad.value = boost::make_optional(to_string_leading_zeroes(counter, leading_zeroes_num));
        else
            dad.value = boost::make_optional(std::to_string(counter));
        value.emplace(std::begin(value), dad);
        bump_counter_up = true;
    }

    if (bump_counter_up)
    {
        counter += renumber_settings.step;
        if (counter > renumber_settings.max_number)
        {
            const auto& it = conv_word_grammar.find(renumber_settings.word);
            if (it != std::end(conv_word_grammar))
                counter = it->second->range_from;
            else
                counter = 1;
        }
    }

    if (renumber_settings.frequency > 1)
    {
        ++current_frequency;
        if (current_frequency == renumber_settings.frequency)
            current_frequency = 0;
    }
}

} // namespace fanuc
} // namespace parser
