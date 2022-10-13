#include "stdafx.h"

#include <iostream>

#include "AttributesVisitor.h"

#include <boost/optional/optional_io.hpp>
#include <boost/variant.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace fanuc_test {

struct CheckTypeVisitor : public boost::static_visitor<>
{
    template <typename T>
    void operator()(const fanuc::AttributeData<T>& value) const
    {
        std::cout << "AttributeData<T> type: " << boost::typeindex::type_id<T>().pretty_name() << std::endl;
    }

    template <typename T, typename U>
    void operator()(const fanuc::AttributeDataDecimal<T, U>& value) const
    {
        std::cout << "AttributeDataDecimal<T,U> types: " << boost::typeindex::type_id<T>().pretty_name() << ", "
                  << boost::typeindex::type_id<U>().pretty_name() << std::endl;
    }
};

MATCHER_P2(IsValueExpected, value_expected, v_expected, "Unsupported type")
{
    if (!value_expected)
    {
        boost::apply_visitor(CheckTypeVisitor(), v_expected);
        return false;
    }

    return true;
}

template <typename T>
bool AttributesVisitor::operator()(const fanuc::AttributeData<T>& value) const
{
    auto value_expected = boost::get<fanuc::AttributeData<T>>(&v_expected);
    EXPECT_THAT(true, IsValueExpected(value_expected, v_expected));

    EXPECT_EQ(value_expected->word, value.word);
    EXPECT_EQ(value_expected->value, value.value);

    return true;
}

bool AttributesVisitor::operator()(const fanuc::DecimalAttributeData& value) const
{
    auto value_expected = boost::get<fanuc::DecimalAttributeData>(&v_expected);
    EXPECT_THAT(true, IsValueExpected(value_expected, v_expected));

    EXPECT_EQ(value_expected->word, value.word);
    EXPECT_THAT(value_expected->assign, value.assign);
    EXPECT_THAT(value_expected->sign, value.sign);
    EXPECT_THAT(value_expected->open_bracket, value.open_bracket);
    EXPECT_THAT(value_expected->macro, value.macro);
    EXPECT_THAT(value_expected->value, value.value);
    EXPECT_THAT(value_expected->dot, value.dot);
    EXPECT_THAT(value.value2, value_expected->value2);
    EXPECT_THAT(value_expected->close_bracket, value.close_bracket);

    return true;
}

template bool AttributesVisitor::operator()(const fanuc::AttributeData<std::string>& value) const;
template bool AttributesVisitor::operator()(const fanuc::AttributeData<char>& value) const;

} // namespace fanuc_test
