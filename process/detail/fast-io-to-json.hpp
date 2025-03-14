#pragma once

#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include <nlohmann/json.hpp>
#include <string>

namespace nlohmann {
template <> struct adl_serializer<fast_io::string> {
    static void to_json(json &j, fast_io::string const &s)
    {
        j = std::string(std::from_range, s);
    }

    static void from_json(json const &j, fast_io::string &s)
    {
        s.assign(
            fast_io::string_view(std::from_range, j.get<std::string_view>()));
    }
};

template <> struct adl_serializer<fast_io::string_view> {
    static void to_json(json &j, fast_io::string_view const &sv)
    {
        j = std::string_view(sv);
    }

    static void from_json(json const &j, fast_io::string_view &sv)
    {
        sv = fast_io::string_view(std::from_range, j.get<std::string_view>());
    }
};
} // namespace nlohmann
