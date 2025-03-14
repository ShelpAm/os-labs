#include <process/time.hpp>

#include <process/detail/fast-io-to-json.hpp>

#include <ranges>
#include <string>

void to_json(nlohmann::json &j, Time const &t)
{
    j = to_string(t);
}

void from_json(nlohmann::json const &j, Time &t)
{
    t = j.get<fast_io::string_view>();
}
