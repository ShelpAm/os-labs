#include <fast_io.h>
#include <nlohmann/json.hpp>
#include <process/process.hpp>
#include <queue>
#include <ranges>
#include <iostream>

int main()
{
    fast_io::native_file_loader f("tests/test_fcfs.json");
    std::string s(f.begin(), f.end());
    // auto data = nlohmann::json::parse(f); // gai zi fu chua
    std::cout<<s;

    // std::string const algorithm{data["algorithm"]};
    // std::vector<Process> jobs;
    // for (auto const &j : data["processes"]) {
    //     Process p(j["id"].get<int>(), j["name"],
    //               Time(j["arrival_time"].get<int>()),
    //               j["total_execution_time"].get<int>());
    //     p.extra = j["extra"];
    //     jobs.push_back(p);
    // }

    // auto const result{route_algorithm(algorithm, jobs)}; // yanzheng res
    // res.set_content(result, "application/json");
    return 0;
}