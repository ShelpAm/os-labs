#include <algorithms/process-scheduling.hpp>

std::vector<Process::Id> to_vector(
    std::priority_queue<Process *, std::vector<Process *>, By_priority> ready)
{
    std::vector<Process::Id> ready_queue;
    while (!ready.empty()) {
        ready_queue.push_back(ready.top()->id);
        ready.pop();
    }
    return ready_queue;
}

std::vector<Process::Id> to_vector(std::queue<Process *> ready)
{
    std::vector<Process::Id> ready_queue;
    while (!ready.empty()) {
        ready_queue.push_back(ready.front()->id);
        ready.pop();
    }
    return ready_queue;
}
std::string solve_first_come_fisrt_serve(CPU cpu, std::vector<Process> jobs)
{
    std::vector<Frame> frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    std::queue<Process *> ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        check_cpu_run_next();
        cpu.run_for(1);
        drive_time_events();
        check_cpu_remove_finished();
        push_frame();
    }

    return nlohmann::json({{"frames", frames}}).dump();
}

std::string solve_short_process_first(CPU cpu, std::vector<Process> jobs)
{
    std::vector<Frame> frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    SPF_queue ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        check_cpu_run_next();
        cpu.run_for(1);
        drive_time_events();
        check_cpu_remove_finished();
        push_frame();
    }

    return nlohmann::json({{"frames", frames}}).dump();
}

std::string solve_round_robin(CPU cpu, std::vector<Process> jobs, int quantum)
{
    std::vector<Frame> frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    std::queue<Process *> ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();           // Drive initial events
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        check_cpu_run_next();
        cpu.run_for(1);
        drive_time_events();
        if (cpu.running_process() != nullptr) {
            if (cpu.running_process()->finished() ||
                cpu.slice_execution_time() >= quantum) {
                if (!cpu.running_process()->finished()) {
                    ready.push(cpu.running_process());
                }
                else {
                    finish_queue.push_back(cpu.running_process()->id);
                }
                cpu.set_running(nullptr);
            }
        }
        push_frame();
    }

    return nlohmann::json({{"frames", frames}}).dump();
}

std::string solve_priority_scheduling(CPU cpu, std::vector<Process> jobs)
{
    std::vector<Frame> frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    Priority_scheduling_queue ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();           // Drive initial events
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        check_cpu_run_next();
        cpu.run_for(1);
        drive_time_events();
        check_cpu_remove_finished();
        push_frame();
    }

    return nlohmann::json({{"frames", frames}}).dump();
}

std::string route_algorithm(std::string_view algorithm,
                            std::vector<Process> jobs)
{
    fast_io::println("Requested algorithm: ", algorithm);
    CPU cpu;
    if (algorithm == "first_come_first_serve") {
        return solve_first_come_fisrt_serve(cpu, jobs);
    }
    if (algorithm == "short_process_first") {
        return solve_short_process_first(cpu, std::move(jobs));
    }
    if (algorithm == "round_robin") {
        return solve_round_robin(cpu, std::move(jobs), 8); // TODO: user should
        // be able to specific any but 8
    }
    if (algorithm == "priority_scheduding") {
        return solve_priority_scheduling(cpu, std::move(jobs));
    }
    throw std::runtime_error(
        std::format("{} hasn't been implemented", algorithm));
}
