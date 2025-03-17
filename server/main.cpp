#include <fast_io.h>
#include <httplib.h>
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
        return lhs->priority() < rhs->priority();
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

std::string route_algorithm(std::string_view algorithm,
                            std::vector<Process> &jobs)
{
    CPU cpu;
    if (algorithm == "priority_scheduding") {
        return solve_priority_scheduling(cpu, jobs);
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
        fast_io::println("Request: ", req.body);
        fast_io::println("Response: ", res.body);
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
