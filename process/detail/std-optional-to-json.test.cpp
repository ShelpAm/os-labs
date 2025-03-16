#include <process/detail/std-optional-to-json.hpp>

struct A {
    std::optional<int> a;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(A, a)
};

void myassert(bool exp, auto &&msg)
{
    if (!exp) {
        std::println("Test failed: {}", msg);
        std::exit(EXIT_FAILURE);
    }
}

int main()
{
    A a;
    myassert(nlohmann::json(a).dump() == R"({"a":null})",
             "nullopt can't be converted to null correctly.");
}
