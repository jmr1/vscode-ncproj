#pragma once

#include <string>
#include <vector>

#include "fanuc/AllAttributesParserDefines.h"

using namespace parser;

namespace fanuc_test {

class AttributesVisitor : public boost::static_visitor<bool>
{
public:
    explicit AttributesVisitor(const fanuc::AttributeVariant& expected)
        : v_expected(expected)
    {
    }

    template <typename T>
    bool operator()(const fanuc::AttributeData<T>& value) const;
    bool operator()(const fanuc::DecimalAttributeData& value) const;

private:
    const fanuc::AttributeVariant& v_expected;
};

} // namespace fanuc_test
