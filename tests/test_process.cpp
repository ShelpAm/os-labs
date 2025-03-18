#include <cstdlib>
#include <fast_io.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <process/process.hpp>
#include <queue>
#include <ranges>
#include <server/algorithms/process-scheduling.hpp>

int main()
{
    fast_io::native_file_loader f("tests/test_fcfs.json");
    std::string s(f.begin(), f.end());
    auto data = nlohmann::json::parse(s); // gai zi fu chua
    std::cout << s << '\n';

    std::string const algorithm{data["algorithm"]};
    std::vector<Process> jobs;
    for (auto const &j : data["processes"]) {
        Process p(j["id"].get<int>(), j["name"],
                  Time(j["arrival_time"].get<int>()),
                  j["total_execution_time"].get<int>());
        p.extra = j["extra"];
        jobs.push_back(p);
    }

    CPU cpu;
    auto const result{solve_first_come_fisrt_serve(cpu, jobs)}; // yanzheng res
    // std::cout<<result.size()<<'\n';
    // result.set_content(result, "application/json");
    nlohmann::json res = nlohmann::json::parse(result);
    nlohmann::json ps1 = res["frames"].back()["processes"];
    nlohmann::json p1 = ps1.back();

    nlohmann::json ps2 = data["processes"];
    std::sort(ps2.begin(), ps2.end());
    nlohmann::json p2 = ps2.back();

    std::cout << ps1 << '\n' << '\n' << ps2 << '\n';

    if (ps1 != ps2) {
        return EXIT_FAILURE;
    }

    return 0;
}