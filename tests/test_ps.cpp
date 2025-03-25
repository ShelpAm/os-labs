#include <algorithms/process-scheduling.hpp>
#include <cstdlib>
#include <fast_io.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <process/process.hpp>
#include <queue>
#include <ranges>
#include <string>

int main()
{
    using namespace std::string_literals;
    for (int k = 1; k <= 2; k++) {
        std::string filename = "tests/test"s + std::to_string(k) + "_ps.json"s;
        fast_io::native_file_loader f(filename);
        std::string s(f.begin(), f.end());
        auto data = nlohmann::json::parse(s); // gai zi fu chua
        // std::cout << s << '\n';

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
        auto const result{solve_priority_scheduling(cpu, jobs)}; // yanzheng res
        // std::cout<<result.size()<<'\n';
        // result.set_content(result, "application/json");
        nlohmann::json res = nlohmann::json(result);
        nlohmann::json ps1 = res["frames"].back()["processes"];
        // nlohmann::json p1 = ps1.back();

        nlohmann::json ps2 = data["processes"];
        std::sort(ps2.begin(), ps2.end());
        // nlohmann::json p2 = ps2.back();

        // std::cout << ps1.size() << ' ' << ps2.size() << '\n';
        // std::cout << ps1 << '\n' << '\n' << ps2 << '\n';

        bool equal = 1;
        double const eps = 0.01;
        for (int i = 0; i < ps1.size(); i++) {
            auto p1 = ps1[i]["stats"], p2 = ps2[i]["stats"];
            auto Relativ_error = [&](double v1, double v2) {
                return std::abs(v1 - v2) / v2;
            };
            double f1 = Relativ_error(p1["start_time"], p2["start_time"]),
                   f2 = Relativ_error(p1["finish_time"], p2["finish_time"]),
                   f3 = Relativ_error(p1["execution_time"],
                                      p2["execution_time"]),
                   f4 = Relativ_error(p1["turnaround"], p2["turnaround"]),
                   f5 = Relativ_error(p1["weighted_turnaround"],
                                      p2["weighted_turnaround"]);

            // std::cout << f1 << ' ' << f2 << ' ' << f3 << ' ' << f4 << ' ' <<
            // f5
            //           << '\n';
            if (f1 > eps || f2 > eps || f3 > eps || f4 > eps || f5 > eps) {
                equal = 0;
            }
        }

        if (!equal) {
            std::cout << "calcuate value = " << ps1 << '\n' << '\n';
            std::cout << "true vaule = " << ps2 << '\n';
            return EXIT_FAILURE;
        }
    }

    return 0;
}