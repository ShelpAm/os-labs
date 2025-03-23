#include <process/time.hpp>

#include <ranges>
#include <string>

void to_json(nlohmann::json &j, Time const &t)
{
    if (t.minutes() != -1) {
        j = t.minutes();
        // j["minutes"] = t.minutes();
    }
}

void from_json(nlohmann::json const &j, Time &t)
{
    t.minutes_ = j.get<int>();
    // t.minutes_ = j["minutes"].get<int>();
}
