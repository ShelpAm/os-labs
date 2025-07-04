#include <algorithms/bankers-algo.hpp>
#include <algorithms/process-scheduling.hpp>
#include <array>
#include <httplib.h>
#include <process/process.hpp>

int find_available_port(std::string host, auto starting_port)
{
    constexpr auto max_usable_port = 65535;
    for (auto i = starting_port; i != max_usable_port + 1; ++i) {
        // fast_io::out() here for flushing
        fast_io::print(fast_io::out(), "Trying to listen on ", host, ":", i,
                       "...");
        httplib::Server svr;
        if (svr.bind_to_port(host, i)) {
            fast_io::println("OK");
            return i;
        }
        fast_io::println("FAILED");
    }
    throw std::runtime_error("failed to find available port");
}

int main()
{
    using httplib::Request, httplib::Response;

    constexpr std::string host{"0.0.0.0"};
    constexpr auto starting_port{8081};

    try {
        auto port = find_available_port(host, starting_port);
        httplib::Server svr;
        svr.set_logger([](Request const &req, Response const &res) {
            fast_io::println("Request: ", req.body);
            fast_io::println("Response: ", res.body);
        });
        svr.set_exception_handler([](Request const &req, Response &res,
                                     std::exception_ptr const &ep) {
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
        auto ret = svr.set_mount_point("/", "./web/dist/");
        svr.Get("/hi", [](Request const &, Response &res) {
            res.set_content("Hello World!", "text/plain");
        });
        svr.Post("/api/solve", [](Request const &req, Response &res) {
            auto data = nlohmann::json::parse(req.body);
            fast_io::println("request: ", data.dump());

            if (data.at("algorithm") == "bankers-algorithm") {
                throw std::runtime_error(
                    "bankers-algorithm is not implemented");
                return;
            }

            using Process = shelpam::scheduling::Process;
            using Request = shelpam::scheduling::Request;
            using Time = shelpam::scheduling::Time;

            auto request = data.get<Request>();
            std::vector<Process> jobs;
            for (auto const &p : request.processes) {
                Process neo(p.id, p.name, p.arrival_time,
                            p.total_execution_time);
                neo.extra = p.extra;
                jobs.push_back(neo);
            }
            auto const result =
                route_algorithm(request.algorithm, jobs, request.extra);
            res.set_content(nlohmann::json(result).dump(), "application/json");

            // std::vector<Process> jobs;
            // for (auto const &j : data["processes"]) {
            //     Process p(j["id"].get<int>(), j["name"],
            //               Time(j["arrival_time"].get<int>()),
            //               // Time(j["arrival_time"]["minutes"].get<int>()),
            //               j["total_execution_time"].get<int>());
            //     p.extra = j["extra"];
            //     jobs.push_back(p);
            // }
            //
            // auto const response = route_algorithm(data["algorithm"], jobs);
            // auto json_result = nlohmann::json(response).dump();
            // // fast_io::println("response: ", json_result);
            // res.set_content(std::move(json_result), "application/json");
        });
        svr.listen(host, port);
    }
    catch (std::exception &e) {
        fast_io::println("\nFailed to bind to ", host, ":", starting_port,
                         " to 65535, exiting.");
    }

    return 0;
}
