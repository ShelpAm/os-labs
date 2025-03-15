#include <process/time.hpp>

#include <ranges>
#include <string>

void to_json(nlohmann::json &j, Time const &t)
{
    // j = to_string(t);
    if (t.minutes() != -1) {
        j = t.minutes();
    }
}

void from_json(nlohmann::json const &j, Time &t)
{
    // t = j.get<fast_io::string_view>();
    t.minutes_ = j.get<int>();
}
