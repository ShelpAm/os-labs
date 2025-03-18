#include <cstdlib>
#include <fast_io.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <process/process.hpp>
#include <queue>
#include <ranges>

struct Frame {
    int system_time;
    std::vector<Process> processes;
    std::optional<Process> running_process;
    std::vector<Process::Id> not_ready_queue;
    std::vector<Process::Id> ready_queue;
    std::vector<Process::Id> finish_queue;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Frame, system_time, processes,
                                   running_process, not_ready_queue,
                                   ready_queue, finish_queue)
};

struct By_priority {
    bool operator()(Process const *lhs, Process const *rhs) const
    {
        return lhs->priority() > rhs->priority();
    }
};

std::vector<Process::Id> to_vector(
    Process const *running,
    std::priority_queue<Process *, std::vector<Process *>, By_priority> ready)
{
    std::vector<Process::Id> ready_queue;
    // if (running != nullptr) {
    //     ready_queue.push_back(running->id);
    // }
    while (!ready.empty()) {
        ready_queue.push_back(ready.top()->id);
        ready.pop();
    }
    return ready_queue;
}

std::string solve_priority_scheduling(CPU &cpu, std::vector<Process> &jobs)
{
    std::vector<Frame> frames;

    auto jobs_it{jobs.begin()};
    std::priority_queue<Process *, std::vector<Process *>, By_priority> ready;
    std::vector<Process::Id> finish_queue;

    auto all_jobs_done{[&jobs_it, &jobs, &ready, &cpu] {
        return jobs_it == jobs.end() && ready.empty() &&
               cpu.running_process() == nullptr;
    }};

    // Check if any job is sent in current time.
    auto drive_time_events{[&]() {
        while (jobs_it != jobs.end() &&
               cpu.system_time() >= jobs_it->arrival_time) {
            ready.push(&*jobs_it);
            ++jobs_it;
        }
    }};

    drive_time_events(); // Drive initial events
    // If no any tasks left, ends the simulation.
    while (!all_jobs_done()) {
        if (cpu.running_process() == nullptr && !ready.empty()) {
            cpu.set_running(ready.top());
            ready.pop();

            // Here needs a frame when some change happens for running_process
            { // Copied from next block of code, change as needed.
                auto const not_ready_queue{
                    std::vector(jobs_it, jobs.end()) |
                    std::views::transform([](auto const &e) { return e.id; }) |
                    std::ranges::to<std::vector>()};
                frames.push_back(
                    {.system_time = cpu.system_time().minutes(),
                     .processes = jobs,
                     .running_process =
                         cpu.running_process() != nullptr
                             ? std::optional<Process>(*cpu.running_process())
                             : std::nullopt,
                     .not_ready_queue = not_ready_queue,
                     .ready_queue = to_vector(cpu.running_process(), ready),
                     .finish_queue = finish_queue});
            }
        }
        cpu.run_for(1);
        drive_time_events();
        if (cpu.running_process() != nullptr &&
            cpu.running_process()->finished()) {
            finish_queue.push_back(cpu.running_process()->id);
            cpu.set_running(nullptr);
        }

        { // Add current frame
            auto const not_ready_queue{
                std::vector(jobs_it, jobs.end()) |
                std::views::transform([](auto const &e) { return e.id; }) |
                std::ranges::to<std::vector>()};
            frames.push_back(
                {.system_time = cpu.system_time().minutes(),
                 .processes = jobs,
                 .running_process =
                     cpu.running_process() != nullptr
                         ? std::optional<Process>(*cpu.running_process())
                         : std::nullopt,
                 .not_ready_queue = not_ready_queue,
                 .ready_queue = to_vector(cpu.running_process(), ready),
                 .finish_queue = finish_queue});
        }
    }

    fast_io::println("Simulation result:");
    output_processes_info(jobs);

    nlohmann::json j;
    j["frames"] = frames;
    return j.dump();
}

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
    auto const result{solve_priority_scheduling(cpu, jobs)}; // yanzheng res
    // std::cout<<result.size()<<'\n';
    // result.set_content(result, "application/json");
    nlohmann::json res = nlohmann::json::parse(result);
    nlohmann::json p1 = res["frames"].back()["processes"].back();
    nlohmann::json p2 = data["processes"].back();
    std::cout << p2 << '\n' << '\n' << p1;
    if (p1 != p2) {
        return EXIT_FAILURE;
    }

    return 0;
}