// Shortest process first

#include <spf/spf-queue.hpp>

#include <fast_io.h>
#include <fast_io_dsal/vector.h>
#include <memory>
#include <process/process.hpp>
#include <ranges>

fast_io::vector<std::shared_ptr<Process>> input()
{
    fast_io::print("Please input the number of processes: ");
    int num_processes{};
    fast_io::scan(num_processes);

    fast_io::vector<std::shared_ptr<Process>> jobs;
    for (int i{}; i != num_processes; ++i) {
        fast_io::println("Please input info of the ", i + 1,
                         "-th process (in an order of id, name, arriving time, "
                         "and execution time):");
        jobs.push_back(std::make_shared<Process>(fast_io::c_stdin()));
    }

    std::ranges::sort(jobs, {}, &Process::arriving_time);

    return jobs;
}

void solve_spf(CPU &cpu, fast_io::vector<std::shared_ptr<Process>> const &jobs)
{
    auto it{jobs.cbegin()};
    SPF_queue ready;

    auto any_jobs_left{[&it, &jobs, &ready, &cpu] {
        return it != jobs.end() || !ready.empty() ||
               cpu.running_process() != nullptr;
    }};

    // If no any tasks left, ends the simulation.
    while (any_jobs_left()) {
        // Check if any job is sent in current time.
        if (it != jobs.end() && cpu.now() == (*it)->arriving_time) {
            ready.enqueue(*it);
            ++it;
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

    output_processes_info(jobs);
}

int main()
{
    CPU cpu;
    auto const jobs{input()};
    solve_spf(cpu, jobs);
}
