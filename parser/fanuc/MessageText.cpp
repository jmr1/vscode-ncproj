#include "stdafx.h"

#include "MessageTextImpl.h"

#include "AllAttributesParserDefines.h"

namespace parser {
namespace fanuc {

static_assert(sizeof(get_message_text_en()) / sizeof(get_message_text_en()[0]) ==
                  sizeof(get_message_text_pl()) / sizeof(get_message_text_pl()[0]),
              "All messages have to be translated");

std::string make_message(const MessageText (&message_text)[static_cast<int>(MessageName::COUNT)], MessageName msg)
{
    std::string str;
    auto        it_msg = std::find_if(std::cbegin(message_text), std::cend(message_text),
                               [&msg](const auto& v) { return v.msg_name == msg; });
    const auto& vmsg   = it_msg->msg_list;
    auto        it     = std::cbegin(vmsg);
    while (it != std::cend(vmsg))
    {
        if (it != std::cend(vmsg))
        {
            str += *it;
            ++it;
        }
    }
    return str;
}

std::string make_message(MessageName name, ELanguage language)
{
    switch (language)
    {
    case ELanguage::English:
        return make_message(get_message_text_en(), name);
    case ELanguage::Polish:
        return make_message(get_message_text_pl(), name);
    }

    return "";
}

} // namespace fanuc
} // namespace parser
