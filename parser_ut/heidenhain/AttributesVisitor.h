#pragma once

#include <string>
#include <vector>

#include "heidenhain/AllAttributesParserDefines.h"

using namespace parser;

namespace heidenhain_test {

class AttributesVisitor : public boost::static_visitor<bool>
{
public:
    explicit AttributesVisitor(const heidenhain::AttributeVariant& expected)
        : v_expected(expected)
    {
    }

    bool operator()(const heidenhain::CommentData& value) const;
    bool operator()(const heidenhain::ProgramBeginEndData& value) const;
    bool operator()(const heidenhain::LineNumberData& value) const;
    bool operator()(const heidenhain::GeneralAttributeData& value) const;
    bool operator()(const heidenhain::CycleDef& value) const;
    bool operator()(const heidenhain::CycleParam& value) const;

private:
    const heidenhain::AttributeVariant& v_expected;
};

} // namespace heidenhain_test
