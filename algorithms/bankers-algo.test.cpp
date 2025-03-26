#include <algorithms/bankers-algo.hpp>
#include <print>
#include <ranges>

void myassert(bool exp, std::invocable<> auto &&fn)
{
    if (!exp) {
        std::println("Test failed, reason:");
        std::invoke(fn);
        std::exit(EXIT_FAILURE);
    }
}

using namespace shelpam::bankers_algo;

constexpr std::array<Resource_kind, 3> resources{"A", "B", "C"};
Resources make_resourece(std::array<std::int32_t, 3> count)
{
    Resources result;
    for (auto [r, c] : std::views::zip(resources, count)) {
        result.count[r] = c;
    }
    return result;
};

int main()
{

    System sys(make_resourece({10, 5, 7}));
    std::println("SYSTEM TOTAL:\n{}", to_string(sys.available()));

    std::vector<Process *> processes;
    auto add_process = [&processes, &sys](std::string name,
                                          std::array<std::int32_t, 3> count) {
        processes.push_back(
            sys.make_process(std::move(name), make_resourece(count)));
    };

    add_process("P1", {7, 5, 3});
    add_process("P2", {3, 2, 2});
    add_process("P3", {9, 0, 2});
    add_process("P4", {2, 2, 2});
    add_process("P5", {4, 3, 3});

    // Initial
    processes[0]->try_request(make_resourece({0, 1, 1}));
    processes[1]->try_request(make_resourece({2, 0, 0}));
    processes[2]->try_request(make_resourece({3, 0, 2}));
    processes[3]->try_request(make_resourece({2, 1, 1}));
    processes[4]->try_request(make_resourece({0, 0, 2}));

    // Subsequent
    processes[0]->try_request(make_resourece({4, 3, 1}));
    processes[0]->try_request(make_resourece({1, 2, 1}));
    processes[0]->try_request(make_resourece({1, 2, 0}));
    processes[3]->try_request(make_resourece({0, 1, 1}));
    processes[1]->try_request(make_resourece({1, 2, 2}));
    processes[2]->try_request(make_resourece({6, 0, 0}));
    processes[4]->try_request(make_resourece({1, 1, 1}));
    processes[0]->try_request(make_resourece({6, 2, 2}));
    processes[4]->try_request(make_resourece({4, 3, 1}));

    std::map<std::string, std::int32_t> last_frame{
        {"A", 10}, {"B", 5}, {"C", 7}};
    myassert(sys.available().count == last_frame, [&count =
                                                       sys.available().count] {
        std::println(
            "In last frame, resources numbers are 10 5 7, but current are:\n");
        for (auto [k, v] : count) {
            std::println("{}: {}", k, v);
        }
    });

    return EXIT_SUCCESS;
}
