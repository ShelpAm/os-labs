#include <algorithms/bankers-algo.hpp>
#include <algorithms/process-scheduling.hpp>
#include <array>
#include <httplib.h>
#include <process/process.hpp>

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
    auto ret = svr.set_mount_point("/", "./web/dist/");
    svr.Get("/hi", [](Request const &, Response &res) {
        res.set_content("Hello World!", "text/plain");
    });
    svr.Post("/api/solve", [](Request const &req, Response &res) {
        auto data = nlohmann::json::parse(req.body);
        fast_io::println("request: ", data.dump());

        if (data.at("algorithm") == "bankers-algorithm") {
            throw std::runtime_error("bankers-algorithm is not implemented");
            return;
        }

        using Process = shelpam::scheduling::Process;
        using Request = shelpam::scheduling::Request;
        using Time = shelpam::scheduling::Time;

        auto request = data.get<Request>();
        std::vector<Process> jobs;
        for (auto const &p : request.processes) {
            Process neo(p.id, p.name, p.arrival_time, p.total_execution_time);
            neo.extra = p.extra;
            jobs.push_back(neo);
        }
        auto const result = route_algorithm(request.algorithm, jobs);
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

    fast_io::println("Starting server on ", host, ":", port);
    if (!svr.listen(host, port)) {
        fast_io::println("Failed to bind to ", host, ":", port, ", exiting.");
    }

    return 0;
}
