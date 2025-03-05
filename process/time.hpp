#pragma once

#include <cassert>
#include <compare>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>

constexpr int to_int(fast_io::string_view s)
{
    constexpr auto base{10};
    int result{};
    for (auto const c : s) {
        result = result * base + (c - '0');
    }
    return result;
}

struct Time {
    constexpr Time() = default;

    constexpr explicit Time(fast_io::string const &s)
    {
        auto const p{s.find_character(':')};
        assert(p != fast_io::npos);
        auto const hour{to_int(s.subview_front(p))};
        auto const minute{to_int(s.subview(p + 1))};
        this->minute = hour * minutes_per_hour + minute;
    }

    constexpr Time &operator=(fast_io::string const &s)
    {
        return *this = Time(s);
    }

    constexpr auto operator<=>(Time const &rhs) const noexcept = default;

    constexpr Time &operator+=(int delta_minutes)
    {
        minute += delta_minutes;
        return *this;
    }

    friend constexpr Time operator+(Time lhs, int delta_minutes)
    {
        return lhs += delta_minutes;
    }

    friend constexpr int operator-(Time const &lhs, Time const &rhs)
    {
        return lhs.minute - rhs.minute;
    }

    static constexpr auto minutes_per_hour{60};
    int minute;
};

constexpr fast_io::string to_string(Time const &t)
{
    auto hour{fast_io::to<fast_io::string>(t.minute / Time::minutes_per_hour)};
    auto minute{
        fast_io::to<fast_io::string>(t.minute % Time::minutes_per_hour)};
    if (hour.size() == 1) {
        hour = fast_io::concat_fast_io(fast_io::mnp::chvw('0'), hour);
    }
    if (minute.size() == 1) {
        minute = fast_io::concat_fast_io(fast_io::mnp::chvw('0'), minute);
    }
    return fast_io::concat_fast_io(hour, fast_io::mnp::chvw(':'), minute);
}
