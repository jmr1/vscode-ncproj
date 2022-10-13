#pragma once

#include "parser_export.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

#include "MessageText.h"

namespace parser {
namespace fanuc {

template <typename... Args>
std::string make_message(const MessageText (&message_text)[static_cast<int>(MessageName::COUNT)], MessageName msg,
                         const Args&... args)
{
    auto        v = {args...};
    std::string str;
    auto        it_msg = std::find_if(std::cbegin(message_text), std::cend(message_text),
                               [&msg](const auto& v) { return v.msg_name == msg; });
    const auto& vmsg   = it_msg->msg_list;
    auto        it1    = std::cbegin(vmsg);
    auto        it2    = std::cbegin(v);
    while (it1 != std::cend(vmsg) || it2 != std::cend(v))
    {
        if (it1 != std::cend(vmsg))
        {
            str += *it1;
            ++it1;
        }
        if (it2 != std::cend(v))
        {
            str += *it2;
            ++it2;
        }
    }
    return str;
}

template <typename... Args>
std::string make_message(MessageName name, ELanguage language, const Args&... args)
{
    switch (language)
    {
    case ELanguage::English:
        return make_message(get_message_text_en(), name, args...);
    case ELanguage::Polish:
        return make_message(get_message_text_pl(), name, args...);
    default:
        return make_message(get_message_text_en(), name, args...);
    }

    return "";
}

template <typename... Args>
std::string make_message2(const MessageText (&message_text)[static_cast<int>(MessageName::COUNT)], MessageName msg,
                          const Args&... args)
{
    auto        v = {args...};
    std::string str;
    auto        it_msg = std::find_if(std::cbegin(message_text), std::cend(message_text),
                               [&msg](const auto& v) { return v.msg_name == msg; });
    const auto& vmsg   = it_msg->msg_list;
    auto        it1    = std::cbegin(vmsg);
    auto        it2    = std::cbegin(v);
    while (it1 != std::cend(vmsg) || it2 != std::cend(v))
    {
        if (it2 != std::cend(v))
        {
            str += *it2;
            ++it2;
        }
        if (it1 != std::cend(vmsg))
        {
            str += *it1;
            ++it1;
        }
    }
    return str;
}

template <typename... Args>
std::string make_message2(MessageName name, ELanguage language, const Args&... args)
{
    switch (language)
    {
    case ELanguage::English:
        return make_message2(get_message_text_en(), name, args...);
    case ELanguage::Polish:
        return make_message2(get_message_text_pl(), name, args...);
    default:
        return make_message2(get_message_text_en(), name, args...);
    }

    return "";
}

template <typename... Args>
std::string make_stream_message(const MessageText (&message_text)[static_cast<int>(MessageName::COUNT)],
                                MessageName msg, const Args&... args)
{
    auto              v = {args...};
    std::stringstream str;
    auto              it_msg = std::find_if(std::cbegin(message_text), std::cend(message_text),
                               [&msg](const auto& v) { return v.msg_name == msg; });
    const auto&       vmsg   = it_msg->msg_list;
    auto              it1    = std::cbegin(vmsg);
    auto              it2    = std::cbegin(v);
    while (it1 != std::cend(vmsg) || it2 != std::cend(v))
    {
        if (it1 != std::cend(vmsg))
        {
            str << *it1;
            ++it1;
        }
        if (it2 != std::cend(v))
        {
            str << *it2;
            ++it2;
        }
    }
    return str.str();
}

template <typename... Args>
std::string make_stream_message(MessageName name, ELanguage language, const Args&... args)
{
    switch (language)
    {
    case ELanguage::English:
        return make_stream_message(get_message_text_en(), name, args...);
    case ELanguage::Polish:
        return make_stream_message(get_message_text_pl(), name, args...);
    default:
        return make_stream_message(get_message_text_en(), name, args...);
    }

    return "";
}

PARSER_API std::string make_message(const MessageText (&message_text)[static_cast<int>(MessageName::COUNT)],
                                    MessageName msg);
PARSER_API std::string make_message(MessageName name, ELanguage language);

} // namespace fanuc
} // namespace parser
