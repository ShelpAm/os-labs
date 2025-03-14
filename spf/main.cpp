// Shortest process first

#include <spf/spf-queue.hpp>

#include <fast_io.h>
#include <fast_io_dsal/vector.h>
#include <memory>
#include <process/process.hpp>

void solve_spf(CPU &cpu, fast_io::vector<Process> &jobs)
{
    auto jobs_it{jobs.begin()};
    SPF_queue ready;

    auto all_jobs_done{[&jobs_it, &jobs, &ready, &cpu] {
        return jobs_it == jobs.end() && ready.empty() &&
               cpu.running_process() == nullptr;
    }};

    // If no any tasks left, ends the simulation.
    while (!all_jobs_done()) {
        // Check if any job is sent in current time.
        while (jobs_it != jobs.end() &&
               cpu.system_time() >= jobs_it->arrival_time) {
            ready.enqueue(jobs_it);
            ++jobs_it;
        }

        if (cpu.running_process() == nullptr && !ready.empty()) {
            cpu.set_running(ready.dequeue());
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
    solve_spf(cpu, jobs);
}
