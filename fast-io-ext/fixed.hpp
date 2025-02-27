#pragma once

#include <concepts>
#include <fast_io_dsal/string.h>

namespace fast_io_ext {

namespace manipulators {

inline fast_io::string fixed(std::floating_point auto t, std::size_t precision)
{
    auto const s{fast_io::to<fast_io::string>(t)};
    if (auto const dot_pos{s.find_character('.')}; dot_pos != fast_io::npos) {
        auto const actual_count{std::min(s.size(), dot_pos + 1 + precision)};
        return s.substr_front(actual_count);
    }
    return s;
}

} // namespace manipulators

namespace mnp = manipulators;

} // namespace fast_io_ext
