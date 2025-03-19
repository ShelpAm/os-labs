#include <nlohmann/json.hpp>
#include <print>

int main()
{
    using json = nlohmann::json;

    json const j = {{"ha", "woqu"}, {"zlx", "zyx"}};
    std::println("j: {}", j.dump());
    for (auto const &[k, v] : j.items()) {
        std::println("{} -> {}", k, v.get<std::string>());
    }
    return 0;
}
