#include <nlohmann/json.hpp>
#include <optional>

// Generic std::optional support
namespace nlohmann {
template <typename T> struct adl_serializer<std::optional<T>> {
    static void to_json(json &j, std::optional<T> const &opt)
    {
        if (opt.has_value()) {
            j = *opt; // Serialize the value
        }
        else {
            j = nullptr; // Serialize as null
        }
    }

    static void from_json(json const &j, std::optional<T> &opt)
    {
        if (j.is_null()) {
            opt = std::nullopt; // Deserialize null as std::nullopt
        }
        else {
            opt = j.get<T>(); // Deserialize value into optional
        }
    }
};
} // namespace nlohmann
