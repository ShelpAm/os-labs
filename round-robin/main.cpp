// Shortest process first

#include <fast_io.h>
#include <memory>
#include <process/process.hpp>
#include <queue>

using namespace shelpam::scheduling;

void solve_round_robin(CPU &cpu, std::vector<Process> &jobs, int quantum)
{
    auto jobs_it{jobs.begin()};
    std::deque<Process *> ready; // Use std::deque for easier output of info

    auto all_jobs_done{[&jobs_it, &jobs, &ready, &cpu] {
        return jobs_it == jobs.end() && ready.empty() &&
               cpu.running_process() == nullptr;
    }};

    // If no any tasks left, ends the simulation.
    int num_process_swtiched{};
    while (!all_jobs_done()) {
        // Check if any job is sent in current time.
        while (jobs_it != jobs.end() &&
               cpu.system_time() >= jobs_it->arrival_time) {
            ready.push_back(&*jobs_it);
            // ready.push(jobs_it);
            ++jobs_it;
        }

        if (cpu.running_process() == nullptr && !ready.empty()) {
            cpu.set_running(ready.front());
            ready.pop_front();
            // ready.pop();
        }
        cpu.run_for(1);
        if (cpu.running_process() != nullptr) {
            if (cpu.running_process()->finished() ||
                cpu.slice_execution_time() >= quantum) {

                fast_io::println(""); // Add empty lines to increase readability
                // Print info
                ++num_process_swtiched;
                fast_io::println(num_process_swtiched,
                                 "-th result of round robin:");
                fast_io::println(
                    "Slice begin time: ",
                    to_string(cpu.system_time() - cpu.slice_execution_time()));
                fast_io::println("Slice end time: ",
                                 to_string(cpu.system_time()));
                fast_io::println("Slice execution time: ",
                                 cpu.slice_execution_time());
                // std::views::concat not implemented. So just wasting some time
                // to add `cpu.running_process()` to the ready queue.
                ready.push_front(cpu.running_process());
                output_processes_info(ready, [](auto const *p) { return *p; });
                ready.pop_front();

                if (!cpu.running_process()->finished()) {
                    ready.push_back(cpu.running_process());
                    // ready.push(cpu.running_process());
                }
                cpu.set_running(nullptr);
            }
        }
    }

    fast_io::println("");
    fast_io::println("Simulation result:");
    // The result is asked for being sorted by finish time.
    // Note that after sorting, all pointers to the jobs will be invalid.
    std::ranges::sort(jobs, {},
                      [](auto const &job) { return job.stats.finish_time; });
    output_processes_info(jobs);
}

int main()
{
    // Constant here for testing
    constexpr auto time_quantum{8};

    CPU cpu;
    auto jobs{input_processes()};
    solve_round_robin(cpu, jobs, time_quantum);
}
