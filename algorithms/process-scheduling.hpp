#pragma once

#include <fast_io.h>
#include <nlohmann/json.hpp>
#include <process/process.hpp>
#include <queue>
#include <ranges>
#include <spf/spf-queue.hpp>

namespace shelpam::scheduling {
namespace experimental { // Not implemented yet
class Scheduler {
  public:
    // Scheduler(std::ranges::range auto workload) :{}
    Scheduler(Scheduler const &) = default;
    Scheduler(Scheduler &&) = delete;
    Scheduler &operator=(Scheduler const &) = default;
    Scheduler &operator=(Scheduler &&) = delete;
    virtual ~Scheduler() = default;
    virtual void schedule() = 0;

  protected:
    // ?? workload_;
};

// First Come, First Served scheduler
class FCFS_scheduler : public Scheduler {};

// Shortest Job First scheduler
class SJF_scheduler : public Scheduler {};

// Shortest Time to Complete First scheduler
class STCF_scheduler : public Scheduler {};

// Round Robin scheduler
class RR_scheduler : public Scheduler {};

// Multi-Level Feedback Queue scheduler
class MLFQ_scheduler : public Scheduler {};
} // namespace experimental

struct Frame {
    int system_time;
    std::vector<Process> processes;
    std::optional<Process> running_process;
    std::vector<Process::Id> not_ready_queue;
    std::vector<Process::Id> ready_queue;
    std::vector<Process::Id> finish_queue;
    nlohmann::json extra;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Frame, system_time, processes,
                                   running_process, not_ready_queue,
                                   ready_queue, finish_queue, extra)
};

enum class Algorithm : std::uint8_t {
    // Process scheduling
    first_come_first_served,
    shortest_process_first,
    shortest_time_to_complete_first,
    round_robin,
    priority_scheduling,
};
std::string_view to_string(Algorithm algorithm);
NLOHMANN_JSON_SERIALIZE_ENUM(
    Algorithm, {
                   {Algorithm::first_come_first_served,
                    to_string(Algorithm::first_come_first_served)},
                   {Algorithm::shortest_process_first,
                    to_string(Algorithm::shortest_process_first)},
                   {Algorithm::shortest_time_to_complete_first,
                    to_string(Algorithm::shortest_time_to_complete_first)},
                   {Algorithm::round_robin, to_string(Algorithm::round_robin)},
                   {Algorithm::priority_scheduling,
                    to_string(Algorithm::priority_scheduling)},
               });

struct Request {
    Algorithm algorithm;
    std::vector<Process> processes;
    nlohmann::json extra;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Request, algorithm, processes, extra);
};

struct Result {
    std::vector<Frame> frames;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Result, frames);

struct By_priority {
    bool operator()(Process const *lhs, Process const *rhs) const
    {
        return lhs->priority() > rhs->priority();
    }
};

struct By_arrival_time {
    bool operator()(Process const *lhs, Process const *rhs) const
    {
        return lhs->arrival_time < rhs->arrival_time;
    }
};

std::vector<Process::Id> to_vector(
    std::priority_queue<Process *, std::vector<Process *>, By_priority> ready);
std::vector<Process::Id> to_vector(std::queue<Process *> ready);

class Priority_scheduling_queue {
    friend std::vector<Process::Id> to_vector(Priority_scheduling_queue ready)
    {
        return to_vector(std::move(ready).queue);
    }

  public:
    // Constructor (default)
    Priority_scheduling_queue() = default;

    // Check if the queue is empty
    [[nodiscard]] bool empty() const
    {
        return queue.empty();
    }

    // Get the size of the queue
    [[nodiscard]] std::size_t size() const
    {
        return queue.size();
    }

    // Mimic std::queue's front: return the highest-priority process (peek)
    [[nodiscard]] Process *front() const
    {
        return queue.top(); // top() gives the highest-priority element
    }

    // Mimic std::queue's push: add a process to the queue
    void push(Process *process)
    {
        queue.push(process);
    }

    // Mimic std::queue's pop: remove the highest-priority process
    void pop()
    {
        queue.pop(); // Removes the highest-priority element
    }

  private:
    std::priority_queue<Process *, std::vector<Process *>, By_priority> queue;
};

#define any_job_unfinished()                                                   \
    [&]() {                                                                    \
        return (jobs_it != jobs.end() || !ready.empty() ||                     \
                cpu.running_process() != nullptr);                             \
    }()

#define drive_time_events()                                                    \
    [&]() {                                                                    \
        while (jobs_it != jobs.end() &&                                        \
               cpu.system_time() >= jobs_it->arrival_time) {                   \
            ready.push(&*jobs_it);                                             \
            ++jobs_it;                                                         \
        }                                                                      \
    }()

#define push_frame()                                                           \
    [&]() {                                                                    \
        auto const not_ready_queue{                                            \
            std::vector(jobs_it, jobs.end()) |                                 \
            std::views::transform([](auto const &e) { return e.id; }) |        \
            std::ranges::to<std::vector>()};                                   \
        frames.push_back({.system_time = cpu.system_time().minutes(),          \
                          .processes = jobs,                                   \
                          .running_process = cpu.running_process() != nullptr  \
                                                 ? std::optional<Process>(     \
                                                       *cpu.running_process()) \
                                                 : std::nullopt,               \
                          .not_ready_queue = not_ready_queue,                  \
                          .ready_queue = to_vector(ready),                     \
                          .finish_queue = finish_queue});                      \
    }()

#define push_frame_extra(extra)                                                \
    [&]() {                                                                    \
        auto const not_ready_queue{                                            \
            std::vector(jobs_it, jobs.end()) |                                 \
            std::views::transform([](auto const &e) { return e.id; }) |        \
            std::ranges::to<std::vector>()};                                   \
        frames.push_back({.system_time = cpu.system_time().minutes(),          \
                          .processes = jobs,                                   \
                          .running_process = cpu.running_process() != nullptr  \
                                                 ? std::optional<Process>(     \
                                                       *cpu.running_process()) \
                                                 : std::nullopt,               \
                          .not_ready_queue = not_ready_queue,                  \
                          .ready_queue = to_vector(ready),                     \
                          .finish_queue = finish_queue,                        \
                          .extra = (extra)});                                  \
    }()

#define check_cpu_set_next()                                                   \
    [&]() {                                                                    \
        if (cpu.running_process() == nullptr && ready.empty())                 \
            cpu.skip_to(jobs_it->arrival_time - 1);                            \
        if (cpu.running_process() == nullptr && !ready.empty()) {              \
            cpu.set_running(ready.front());                                    \
            ready.pop();                                                       \
            push_frame();                                                      \
        }                                                                      \
    }()

#define check_cpu_set_next_extra(extra)                                        \
    [&]() {                                                                    \
        /* If there is no running process, skip to the nearest. */             \
        if (cpu.running_process() == nullptr && ready.empty())                 \
            cpu.skip_to(jobs_it->arrival_time - 1);                            \
        if (cpu.running_process() == nullptr && !ready.empty()) {              \
            cpu.set_running(ready.front());                                    \
            ready.pop();                                                       \
            push_frame_extra(extra);                                           \
        }                                                                      \
    }()

#define check_cpu_remove_finished()                                            \
    [&]() {                                                                    \
        if (cpu.running_process() != nullptr &&                                \
            cpu.running_process()->finished()) {                               \
            finish_queue.push_back(cpu.running_process()->id);                 \
            cpu.set_running(nullptr);                                          \
        }                                                                      \
    }()

using Frame_list = std::vector<Frame>;
Frame_list solve_first_come_fisrt_served(CPU cpu, std::vector<Process> jobs);
Frame_list solve_shortest_process_first(CPU cpu, std::vector<Process> jobs);
Frame_list solve_round_robin(CPU cpu, std::vector<Process> jobs, int quantum);
Frame_list solve_priority_scheduling(CPU cpu, std::vector<Process> jobs);

Result route_algorithm(Algorithm algorithm, std::vector<Process> jobs,
                       nlohmann::json const &extra);

} // namespace shelpam::scheduling
