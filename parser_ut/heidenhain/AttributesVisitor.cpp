#include "stdafx.h"

#include <iostream>

#include "AttributesVisitor.h"

#include <boost/optional/optional_io.hpp>
#include <boost/variant.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace heidenhain_test {

struct CheckTypeVisitor : public boost::static_visitor<>
{
    template <typename T>
    void operator()(const T& value) const
    {
        std::cout << "Type: " << boost::typeindex::type_id<T>().pretty_name() << std::endl;
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

bool AttributesVisitor::operator()(const heidenhain::LineNumberData& value) const
{
    auto value_expected = boost::get<heidenhain::LineNumberData>(&v_expected);
    EXPECT_THAT(true, IsValueExpected(value_expected, v_expected));

    EXPECT_EQ(value_expected->value, value.value);

    return true;
}

bool AttributesVisitor::operator()(const heidenhain::CycleDef& value) const
{
    auto value_expected = boost::get<heidenhain::CycleDef>(&v_expected);
    EXPECT_THAT(true, IsValueExpected(value_expected, v_expected));

    EXPECT_EQ(value_expected->word1, value.word1);
    EXPECT_EQ(value_expected->word2, value.word2);
    EXPECT_THAT(value_expected->value, value.value);
    EXPECT_THAT(value_expected->dot, value.dot);
    EXPECT_THAT(value_expected->value2, value.value2);

    return true;
}

bool AttributesVisitor::operator()(const heidenhain::CycleParam& value) const
{
    auto value_expected = boost::get<heidenhain::CycleParam>(&v_expected);
    EXPECT_THAT(true, IsValueExpected(value_expected, v_expected));

    EXPECT_EQ(value_expected->name, value.name);
    EXPECT_EQ(value_expected->id, value.id);
    EXPECT_EQ(value_expected->assign, value.assign);

    EXPECT_THAT(value_expected->sign, value.sign);
    EXPECT_EQ(value_expected->value, value.value);
    EXPECT_THAT(value_expected->dot, value.dot);
    EXPECT_THAT(value_expected->value2, value.value2);

    return true;
}

bool AttributesVisitor::operator()(const heidenhain::CommentData& value) const
{
    auto value_expected = boost::get<heidenhain::CommentData>(&v_expected);
    EXPECT_THAT(true, IsValueExpected(value_expected, v_expected));

    EXPECT_EQ(value_expected->semicolon, value.semicolon);
    EXPECT_EQ(value_expected->value, value.value);
    EXPECT_EQ(value_expected->tilde, value.tilde);

    return true;
}

bool AttributesVisitor::operator()(const heidenhain::ProgramBeginEndData& value) const
{
    auto value_expected = boost::get<heidenhain::ProgramBeginEndData>(&v_expected);
    EXPECT_THAT(true, IsValueExpected(value_expected, v_expected));

    EXPECT_EQ(static_cast<std::underlying_type<decltype(value_expected->begin_end)>::type>(value_expected->begin_end),
              static_cast<std::underlying_type<decltype(value.begin_end)>::type>(value.begin_end));
    EXPECT_EQ(value_expected->name, value.name);
    EXPECT_EQ(static_cast<std::underlying_type<decltype(value_expected->unit)>::type>(value_expected->unit),
              static_cast<std::underlying_type<decltype(value.unit)>::type>(value.unit));

    return true;
}

bool AttributesVisitor::operator()(const heidenhain::GeneralAttributeData& value) const
{
    auto value_expected = boost::get<heidenhain::GeneralAttributeData>(&v_expected);
    EXPECT_THAT(true, IsValueExpected(value_expected, v_expected));

    EXPECT_EQ(value_expected->word, value.word);
    EXPECT_THAT(value_expected->sign, value.sign);
    EXPECT_THAT(value_expected->name, value.name);
    EXPECT_THAT(value_expected->value, value.value);
    EXPECT_THAT(value_expected->dot, value.dot);
    EXPECT_THAT(value_expected->value2, value.value2);
    EXPECT_THAT(value_expected->tilde, value.tilde);

    return true;
}

} // namespace heidenhain_test
