#pragma once

#include <process/cpu.hpp>
#include <process/detail/fast-io-to-json.hpp>
#include <process/time.hpp>

#include <compare>
#include <cstddef>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
#include <map>
#include <nlohmann/json.hpp>
#include <ranges>
#include <stdexcept>
#include <utility>

enum Status : std::uint8_t { not_started, ready, running, finished };

NLOHMANN_JSON_SERIALIZE_ENUM(Status, {
                                         {not_started, "not_started"},
                                         {ready, "ready"},
                                         {running, "running"},
                                         {finished, "finished"},
                                     })

struct Process {
    using Id = int;

    struct Runtime_info {
        Time start_time;
        Time finish_time;
        // executed_time + remaining_time == execution_time
        int execution_time{};
        int remaining_time;
        int turnaround;             // 周转时间
        double weighted_turnaround; // 带权周转时间
        Status status;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Runtime_info, start_time, finish_time,
                                       execution_time, remaining_time,
                                       turnaround, weighted_turnaround, status)

        constexpr auto operator<=>(Runtime_info const &) const = default;
    };

    // Intrinsic properties
    Id id{};
    std::string name;
    Time arrival_time;
    int total_execution_time{};
    int priority{};

    // Non-intrinsic properties
    Runtime_info runtime_info{};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Process, id, name, arrival_time,
                                   total_execution_time, priority, runtime_info)

    [[deprecated("Only for nlohmann/json, don't use it")]] constexpr Process() =
        default;

    constexpr Process(int id, std::string name, Time arrival_time,
                      int total_execution_time)
        : id(id), name(std::move(name)), arrival_time(arrival_time),
          total_execution_time(total_execution_time)
    {
        runtime_info.remaining_time = total_execution_time;
    }

    constexpr explicit Process(fast_io::c_io_observer in)
    {
        fast_io::string arriving_time;
        fast_io::scan(in, id, name, arriving_time, total_execution_time);
        this->arrival_time =
            fast_io::string_view(std::from_range, arriving_time);
        this->runtime_info.remaining_time = total_execution_time;
    }

    constexpr auto operator<=>(Process const &) const = default;

    [[nodiscard]] constexpr bool started() const noexcept
    {
        return runtime_info.execution_time != 0;
    }

    [[nodiscard]] constexpr bool finished() const noexcept
    {
        return runtime_info.execution_time == total_execution_time;
    }

    // Returns: actual running time of current running process.
    constexpr int run_for(int minutes, CPU const &cpu)
    {
        if (finished()) {
            throw std::logic_error(
                fast_io::concat_fast_io(
                    "cannot run this process, it has been finished ", name, " ",
                    runtime_info.execution_time)
                    .c_str());
        }

        if (!started()) {
            runtime_info.start_time = cpu.system_time();
        }

        // Running time shouldn't exceed remaining execution time.
        minutes = std::min(minutes, runtime_info.remaining_time);

        // fast_io::debug_println("Process ", id, " ", name,
        //                        " is now running (for ", minutes, "min)");
        runtime_info.execution_time += minutes;
        runtime_info.remaining_time -= minutes;
        assert(runtime_info.execution_time + runtime_info.remaining_time ==
               total_execution_time);

        if (finished()) {
            runtime_info.finish_time = cpu.system_time() + minutes;
            calculate_process_stats();
        }

        return minutes;
    }

  private:
    constexpr void calculate_process_stats()
    {
        runtime_info.turnaround = runtime_info.finish_time - arrival_time;

        runtime_info.weighted_turnaround = runtime_info.turnaround;
        runtime_info.weighted_turnaround /= total_execution_time;
    }
};

std::size_t count_chinese_characters(std::string_view str);

fast_io::vector<Process> input_processes();

template <std::ranges::range Range,
          std::invocable<std::ranges::range_value_t<Range>> Projection =
              std::identity>
    requires std::same_as<std::remove_cvref_t<std::invoke_result_t<
                              Projection, std::ranges::range_value_t<Range>>>,
                          Process>
void output_processes_info(Range &&_processes, Projection &&_proj = {})
{
    auto processes{std::forward<Range>(_processes) |
                   std::views::transform(std::forward<Projection>(_proj))};

    static std::vector<std::string_view> const items{"ID",
                                                     "Name",
                                                     "Arrival time",
                                                     "Total execution time",
                                                     "Priority",
                                                     "Start time",
                                                     "Finish time",
                                                     "Turnaround time",
                                                     "Weighted turnaround time",
                                                     "Execution time"};
    // Generate mapping from `items`.
    static std::map<std::string_view, std::size_t> const index_of_item{[] {
        std::map<std::string_view, std::size_t> index_of_item;
        for (int i{}; auto const &item : items) {
            index_of_item[item] = i++;
        }
        return index_of_item;
    }()};
    std::string const padding(6, ' ');
    std::vector<std::size_t> offsets;
    // item1<3 spaces>item2<3 spaces>...
    std::string heading;
    for (std::size_t offset{}; auto const &item : items) {
        std::string part{item};
        part += padding;
        heading += part;
        offsets.push_back(offset);
        offset += part.size();
    }

    std::vector<std::string> lines(processes.size());
    for (auto [p, buffer] : std::views::zip(processes, lines)) {
        int items_i{};
        int offset_for_chinese{};

        auto align_next{[&buffer, &items_i, &offsets, &offset_for_chinese] {
            // Slow here, but no other better choice because fast_io cannot
            // build string by char and its count. See
            // https://github.com/cppfastio/fast_io/issues/1104
            while (buffer.size() + offset_for_chinese < offsets[items_i]) {
                buffer.push_back(' ');
            }
            ++items_i;
        }};

        auto add_next_item{[&](std::string s) {
            align_next();
            buffer.append(std::move(s));
        }};

        // Prepare info of lines
        std::map<std::string_view, std::string> map;
        {
            using std::to_string;
            map["ID"] = to_string(p.id);
            map["Name"] = p.name;
            map["Arrival time"] = to_string(p.arrival_time);
            map["Total execution time"] = to_string(p.total_execution_time);
            map["Priority"] = to_string(p.priority);
            map["Start time"] =
                to_string(p.runtime_info.start_time, "Not started");
            map["Finish time"] =
                to_string(p.runtime_info.finish_time, "Not finished");
            map["Turnaround time"] = to_string(p.runtime_info.turnaround);
            map["Weighted turnaround time"] =
                std::format("{:.2f}", p.runtime_info.weighted_turnaround);
            map["Execution time"] = to_string(p.runtime_info.execution_time);
        }

        // Build the line
        for (auto const &item : items) {
            add_next_item(map[item]);
            offset_for_chinese +=
                -static_cast<int>(count_chinese_characters(map[item]));
        }
    }

    fast_io::println(heading);
    for (auto const &line : lines) {
        fast_io::println(line);
    }

    double average_turnaround{};
    double average_weighted_turnaround{};
    for (auto const &p : processes) {
        average_turnaround += p.runtime_info.turnaround;
        average_weighted_turnaround += p.runtime_info.weighted_turnaround;
    }
    average_turnaround /= static_cast<double>(processes.size());
    average_weighted_turnaround /= static_cast<double>(processes.size());

    {
        using namespace std::string_view_literals;
        fast_io::println(
            "Average turnaround time of the system: ",
            std::string(offsets[index_of_item.at("Turnaround time")] -
                            "Average turnaround time of the system: "sv.size(),
                        ' '),
            std::format("{:.2f}", average_turnaround, 2));
        fast_io::println(
            "Average weighted turnaround time of the system: ",
            std::string(
                offsets[index_of_item.at("Weighted turnaround time")] -
                    "Average weighted turnaround time of the system: "sv.size(),
                ' '),
            std::format("{:.2f}", average_weighted_turnaround, 2));
    }
}
