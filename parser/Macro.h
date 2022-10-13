#pragma once

#define LEFT_STREAM_OP_DECL(TYPE) PARSER_API std::ostream& operator<<(std::ostream& ostr, TYPE e);

#define RIGHT_STREAM_OP_DECL(TYPE) PARSER_API std::istream& operator>>(std::istream& istr, TYPE& e);

#define LEFT_STREAM_OP(TYPE)                                                                                           \
    std::ostream& operator<<(std::ostream& ostr, TYPE e)                                                               \
    {                                                                                                                  \
        ostr << to_string(e);                                                                                          \
        return ostr;                                                                                                   \
    }

#define RIGHT_STREAM_OP(TYPE)                                                                                          \
    std::istream& operator>>(std::istream& istr, TYPE& e)                                                              \
    {                                                                                                                  \
        std::string s;                                                                                                 \
        istr >> s;                                                                                                     \
        e = to_##TYPE(s);                                                                                              \
        return istr;                                                                                                   \
    }
