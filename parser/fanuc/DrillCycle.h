#pragma once

#include <algorithm>
#include <array>

#include "GeneralParserDefines.h"

namespace parser {
namespace fanuc {

constexpr std::array<EDrillGmode, 8> drill_cycle = {EDrillGmode::G74, EDrillGmode::G81, EDrillGmode::G82,
                                                    EDrillGmode::G84, EDrillGmode::G85, EDrillGmode::G86,
                                                    EDrillGmode::G88, EDrillGmode::G89};

constexpr std::array<EDrillGmode, 4> drill_cycle_qparam = {EDrillGmode::G73, EDrillGmode::G76, EDrillGmode::G83,
                                                           EDrillGmode::G87};

constexpr bool is_drill_cycle(int value)
{
    return std::any_of(std::cbegin(drill_cycle), std::cend(drill_cycle),
                       [value](EDrillGmode gmode) { return static_cast<int>(gmode) == value; });
}

constexpr bool is_drill_cycle_qparam(int value)
{
    return std::any_of(std::cbegin(drill_cycle_qparam), std::cend(drill_cycle_qparam),
                       [value](EDrillGmode gmode) { return static_cast<int>(gmode) == value; });
}

} // namespace fanuc
} // namespace parser