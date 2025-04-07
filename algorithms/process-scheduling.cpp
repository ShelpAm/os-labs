#include <algorithms/process-scheduling.hpp>

using namespace shelpam;
using Process = scheduling::Process;
using Frame_list = scheduling::Frame_list;
using Result = scheduling::Result;

std::vector<scheduling::Process::Id>
scheduling::to_vector(std::priority_queue<Process *, std::vector<Process *>,
                                          scheduling::By_priority>
                          ready)
{
    std::vector<Process::Id> ready_queue;
    while (!ready.empty()) {
        ready_queue.push_back(ready.top()->id);
        ready.pop();
    }
    return ready_queue;
}

std::vector<Process::Id> scheduling::to_vector(std::queue<Process *> ready)
{
    std::vector<Process::Id> ready_queue;
    while (!ready.empty()) {
        ready_queue.push_back(ready.front()->id);
        ready.pop();
    }
    return ready_queue;
}

Frame_list scheduling::solve_first_come_fisrt_served(CPU cpu,
                                                     std::vector<Process> jobs)
{
    Frame_list frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    std::queue<Process *> ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        check_cpu_set_next();
        cpu.run_for(1);
        drive_time_events();
        check_cpu_remove_finished();
        push_frame();
    }

    return frames;
}

Frame_list scheduling::solve_shortest_process_first(CPU cpu,
                                                    std::vector<Process> jobs)
{
    std::vector<Frame> frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    SPF_queue ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        check_cpu_set_next();
        cpu.run_for(1);
        drive_time_events();
        check_cpu_remove_finished();
        push_frame();
    }

    return frames;
}

Frame_list scheduling::solve_round_robin(CPU cpu, std::vector<Process> jobs,
                                         int quantum)
{
    std::vector<Frame> frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    std::queue<Process *> ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();           // Drive initial events
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        auto extra = nlohmann::json(
            {{"cpu.slice_execution_time", cpu.slice_execution_time()}});
        check_cpu_set_next_extra(extra);
        cpu.run_for(1);
        extra = nlohmann::json(
            {{"cpu.slice_execution_time", cpu.slice_execution_time()}});
        push_frame_extra(extra);
        drive_time_events();
        if (cpu.running_process() != nullptr) {
            if (cpu.running_process()->finished() ||
                cpu.slice_execution_time() >= quantum) {
                if (!cpu.running_process()->finished()) {
                    ready.push(cpu.running_process());
                }
                else { // Next
                    finish_queue.push_back(cpu.running_process()->id);
                }
                cpu.set_running(nullptr);
            }
        }
        extra = nlohmann::json(
            {{"cpu.slice_execution_time", cpu.slice_execution_time()}});
        push_frame_extra(extra);
    }

    return frames;
}

Frame_list scheduling::solve_priority_scheduling(CPU cpu,
                                                 std::vector<Process> jobs)
{
    std::vector<Frame> frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    Priority_scheduling_queue ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();           // Drive initial events
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        check_cpu_set_next();
        cpu.run_for(1);
        drive_time_events();
        check_cpu_remove_finished();
        push_frame();
    }

    return frames;
}

Frame_list
scheduling::solve_priority_scheduling_preemptive(CPU cpu,
                                                 std::vector<Process> jobs)
{
    std::vector<Frame> frames;

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    auto jobs_it{jobs.begin()};
    Priority_scheduling_queue ready;
    std::vector<Process::Id> finish_queue;

    drive_time_events();           // Drive initial events
    while (any_job_unfinished()) { // If no any tasks left, ends the simulation.
        check_cpu_set_next_preemptive();
        cpu.run_for(1);
        drive_time_events();
        check_cpu_remove_finished();
        push_frame();
    }

    return frames;
}

Result scheduling::route_algorithm(Algorithm algorithm,
                                   std::vector<Process> jobs,
                                   nlohmann::json const &extra)
{
    std::println("routing to {}", to_string(algorithm));
    Result response;
    switch (algorithm) {
    case Algorithm::unknown:
        throw std::runtime_error(
            std::format("{} hasn't been implemented", to_string(algorithm)));
    case Algorithm::first_come_first_served: {
        CPU cpu;
        response.frames = solve_first_come_fisrt_served(cpu, std::move(jobs));
        return response;
    }
    case Algorithm::shortest_process_first: {
        CPU cpu;
        response.frames = solve_shortest_process_first(cpu, std::move(jobs));
        return response;
    }
    case Algorithm::shortest_time_to_complete_first:
        throw std::runtime_error(
            std::format("{} hasn't been implemented", to_string(algorithm)));
    case Algorithm::round_robin: {
        CPU cpu;
        response.frames =
            solve_round_robin(cpu, std::move(jobs), extra.at("time_quantum"));
        return response;
    }
    case Algorithm::priority_scheduling: {
        CPU cpu;
        response.frames = solve_priority_scheduling(cpu, std::move(jobs));
        return response;
    }
    case Algorithm::priority_scheduling_preemptive:
        CPU cpu;
        response.frames =
            solve_priority_scheduling_preemptive(cpu, std::move(jobs));
        return response;
    }
}

std::string_view scheduling::to_string(Algorithm algorithm)
{
    static std::unordered_map<Algorithm, std::string_view> const map{
        {Algorithm::first_come_first_served, "first_come_first_served"},
        {Algorithm::shortest_process_first, "shortest_process_first"},
        {Algorithm::shortest_time_to_complete_first,
         "shortest_time_to_complete_first"},
        {Algorithm::round_robin, "round_robin"},
        {Algorithm::priority_scheduling, "priority_scheduling"},
        {Algorithm::priority_scheduling_preemptive,
         "priority_scheduling_preemptive"},
    };
    return map.at(algorithm);
}
