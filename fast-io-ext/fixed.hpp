#pragma once

#include <cmath>
#include <concepts>
#include <fast_io_dsal/string.h>

namespace {

// 满足"四舍六入五成双"规则
inline auto round(std::floating_point auto f, std::size_t precision)
{
    auto const pow{std::pow(10, precision)};
    auto const tmp{f * pow};
    auto const frac{tmp - std::floor(tmp)};

    constexpr auto half{0.5};
    if (frac == half) {
        return (static_cast<std::int_least64_t>(tmp) % 2 == 1
                    ? std::ceil(tmp)
                    : std::floor(tmp)) /
               pow;
    }
    return std::round(tmp) / pow;
}

} // namespace

namespace fast_io_ext {

namespace manipulators {

[[deprecated("Don't use fast_io data structures, use "
             "std::format(\"{:.<precision>f}\", <t>)")]] inline fast_io::string
fixed(std::floating_point auto t, std::size_t precision)
{
    auto const s{fast_io::to<fast_io::string>(round(t, precision))};
    if (auto const dot_pos{s.find_character('.')}; dot_pos != fast_io::npos) {
        auto const actual_count{std::min(s.size(), dot_pos + 1 + precision)};
        return s.substr_front(actual_count);
    }
    return s;
}

} // namespace manipulators

namespace mnp = manipulators;

} // namespace fast_io_ext
