// Priority scheduling

#include <fast_io.h>
#include <process/process.hpp>
#include <queue>

struct By_priority {
    constexpr bool operator()(Process const *lhs, Process const *rhs) const
    {
        return lhs->priority() < rhs->priority();
    }
};

void solve_priority_scheduling(CPU &cpu, std::vector<Process> &jobs)
{
    auto jobs_it{jobs.begin()};
    std::priority_queue<Process *, fast_io::vector<Process *>, By_priority>
        ready;

    auto all_jobs_done{[&jobs_it, &jobs, &ready, &cpu] {
        return jobs_it == jobs.end() && ready.empty() &&
               cpu.running_process() == nullptr;
    }};

    // If no any tasks left, ends the simulation.
    while (!all_jobs_done()) {
        // Check if any job is sent in current time.
        while (jobs_it != jobs.end() &&
               cpu.system_time() >= jobs_it->arrival_time) {
            ready.push(&*jobs_it);
            ++jobs_it;
        }

        if (cpu.running_process() == nullptr && !ready.empty()) {
            cpu.set_running(ready.top());
            ready.pop();
        }
        cpu.run_for(1);
        if (cpu.running_process() != nullptr &&
            cpu.running_process()->finished()) {
            cpu.set_running(nullptr);
        }
    }

    fast_io::println("Simulation result:");
    output_processes_info(jobs);
}

int main()
{
    CPU cpu;
    auto jobs{input_processes()};
    solve_priority_scheduling(cpu, jobs);
}
