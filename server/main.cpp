#include <fast_io.h>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <process/process.hpp>
#include <queue>
#include <ranges>
#include <spf/spf-queue.hpp>

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

#define check_cpu_run_next()                                                   \
    [&]() {                                                                    \
        if (cpu.running_process() == nullptr && !ready.empty()) {              \
            cpu.set_running(ready.front());                                    \
            ready.pop();                                                       \
            push_frame();                                                      \
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

int main()
{
    using httplib::Request, httplib::Response;

    constexpr std::string host{"0.0.0.0"};
    constexpr auto port{8081};

    httplib::Server svr;
    svr.set_logger([](Request const &req, Response const &res) {
        // fast_io::println("Request: ", req.body);
        // fast_io::println("Response: ", res.body);
    });
    svr.set_exception_handler(
        [](Request const &req, Response &res, std::exception_ptr const &ep) {
            std::string error;
            try {
                std::rethrow_exception(ep);
            }
            catch (std::exception &e) {
                error = e.what();
                fast_io::println("Error: ", fast_io::mnp::os_c_str(e.what()));
            }
            res.set_content(error, "text/plain");
            res.status = httplib::StatusCode::InternalServerError_500;
        });
    auto ret = svr.set_mount_point("/", "./web/public/");
    svr.Get("/hi", [](Request const &, Response &res) {
        res.set_content("Hello World!", "text/plain");
    });
    svr.Post("/api/solve", [](Request const &req, Response &res) {
        auto data = nlohmann::json::parse(req.body);

        std::string const algorithm{data["algorithm"]};
        std::vector<Process> jobs;
        for (auto const &j : data["processes"]) {
            Process p(j["id"].get<int>(), j["name"],
                      Time(j["arrival_time"].get<int>()),
                      j["total_execution_time"].get<int>());
            p.extra = j["extra"];
            jobs.push_back(p);
        }

        auto const result{route_algorithm(algorithm, jobs)};
        res.set_content(result, "application/json");
    });

    fast_io::println("Starting server on ", host, ":", port);
    if (!svr.listen(host, port)) {
        fast_io::println("Failed to bind to ", host, ":", port, ", exiting.");
    }

    return 0;
}
