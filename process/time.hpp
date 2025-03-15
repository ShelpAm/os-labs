#pragma once

#include <cassert>
#include <compare>
#include <fast_io.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include <nlohmann/json.hpp>

// params:
// s: string-like
constexpr int to_int(auto const &s)
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

    constexpr explicit Time(int minutes) : minutes_{minutes} {}

    constexpr explicit Time(std::string_view s)
    {
        auto const p{s.find(':')};
        assert(p != std::string::npos);
        auto const hour{to_int(s.substr(0, p))};
        auto const minute{to_int(s.substr(p + 1))};
        this->minutes_ = hour * minutes_per_hour + minute;
    }

    constexpr explicit Time(fast_io::string_view s)
    {
        auto const p{s.find_character(':')};
        assert(p != fast_io::npos);
        auto const hour{to_int(s.subview_front(p))};
        auto const minute{to_int(s.subview(p + 1))};
        this->minutes_ = hour * minutes_per_hour + minute;
    }

    constexpr Time &operator=(fast_io::string_view s)
    {
        return *this = Time(s);
    }

    constexpr auto operator<=>(Time const &) const = default;

    constexpr Time &operator+=(int delta_minutes)
    {
        minutes_ += delta_minutes;
        return *this;
    }

    constexpr Time &operator-=(int delta_minutes)
    {
        minutes_ -= delta_minutes;
        return *this;
    }

    friend constexpr Time operator+(Time lhs, int delta_minutes)
    {
        return lhs += delta_minutes;
    }

    friend constexpr int operator-(Time const &lhs, Time const &rhs)
    {
        return lhs.minutes_ - rhs.minutes_;
    }

    friend constexpr Time operator-(Time lhs, int delta_minutes)
    {
        return lhs -= delta_minutes;
    }

    [[nodiscard]] int minutes() const
    {
        return minutes_;
    }

    static constexpr auto minutes_per_hour{60};
    int minutes_{-1};
};

// When t.minute is -1, returns `defaulted`.
constexpr std::string to_string(Time const &t, std::string_view fallback = "")
{
    if (t.minutes_ == -1) {
        return std::string(fallback);
    }

    auto hour{std::to_string(t.minutes_ / Time::minutes_per_hour)};
    auto minute{std::to_string(t.minutes_ % Time::minutes_per_hour)};
    if (hour.size() == 1) {
        hour = fast_io::concat_std(fast_io::mnp::chvw('0'), hour);
    }
    if (minute.size() == 1) {
        minute = fast_io::concat_std(fast_io::mnp::chvw('0'), minute);
    }
    return fast_io::concat_std(hour, fast_io::mnp::chvw(':'), minute);
}

void to_json(nlohmann::json &j, Time const &t);
void from_json(nlohmann::json const &j, Time &t);
