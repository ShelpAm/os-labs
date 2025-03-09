#pragma once

#include <process/cpu.hpp>
#include <process/time.hpp>

#include <compare>
#include <cstddef>
#include <fast-io-ext/fixed.hpp>
#include <fast_io.h>
#include <fast_io_dsal/span.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/vector.h>
#include <map>
#include <ranges>
#include <stdexcept>

struct Process {
    struct Runtime_info {
        Time start_time;
        Time finish_time;
        int turnaround;            // 周转时间
        double weighed_turnaround; // 带权周转时间

        // Runtime info
        // executed_time + remaining_time == execution_time
        int execution_time{};
        int remaining_time;

        constexpr auto operator<=>(Runtime_info const &) const = default;
    };

    // Intrinsic properties
    int id{};
    fast_io::string name;
    Time arrival_time{};
    int total_execution_time{};

    // Non-intrinsic properties
    Runtime_info runtime_info{};

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

        runtime_info.weighed_turnaround = runtime_info.turnaround;
        runtime_info.weighed_turnaround /= total_execution_time;
    }
};

std::size_t count_chinese_characters(fast_io::string const &str);

fast_io::vector<Process> input_processes();

template <std::ranges::range Range,
          std::invocable<std::ranges::range_value_t<Range>> Projection =
              std::identity>
    requires std::same_as<std::remove_cvref_t<std::invoke_result_t<
                              Projection, std::ranges::range_value_t<Range>>>,
                          Process>
void output_processes_info(Range &&_processes, Projection &&_projection = {})
{
    auto &&processes{
        std::forward<Range>(_processes) |
        std::views::transform(std::forward<Projection>(_projection))};

    static fast_io::vector<fast_io::string_view> const items{
        "ID",
        "Name",
        "Arrival time",
        "Total execution time",
        "Start time",
        "Finish time",
        "Turnaround time",
        "Weighed turnaround time",
        "Execution time"};
    // Generate mapping from `items`.
    static std::map<fast_io::string_view, std::size_t> const index_of_item{[] {
        std::map<fast_io::string_view, std::size_t> index_of_item;
        for (int i{}; auto const &item : items) {
            index_of_item[item] = i++;
        }
        return index_of_item;
    }()};
    std::string const padding(6, ' ');
    fast_io::vector<std::size_t> offsets;
    // item1<3 spaces>item2<3 spaces>...
    fast_io::string heading;
    for (std::size_t offset{}; auto const &item : items) {
        fast_io::string part{item};
        part.append(fast_io::mnp::os_c_str(padding.c_str()));
        heading.append(part);
        offsets.push_back(offset);
        offset += part.size();
    }

    fast_io::vector<fast_io::string> lines(processes.size());
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

        auto add_next_item{[&](fast_io::string const &s) {
            align_next();
            buffer.append(s);
        }};

        // Prepare info of lines
        std::map<fast_io::string_view, fast_io::string> map;
        {
            using fast_io::string;
            using fast_io::to;
            map["ID"] = to<string>(p.id);
            map["Name"] = p.name;
            map["Arrival time"] = to_string(p.arrival_time);
            map["Total execution time"] = to<string>(p.total_execution_time);
            map["Start time"] = to_string(p.runtime_info.start_time);
            map["Finish time"] = to_string(p.runtime_info.finish_time);
            map["Turnaround time"] = to<string>(p.runtime_info.turnaround);
            map["Weighed turnaround time"] =
                fast_io_ext::mnp::fixed(p.runtime_info.weighed_turnaround, 2);
            map["Execution time"] = to<string>(p.runtime_info.execution_time);
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

    double average_turnaround_time{};
    double average_weighed_turnaround_time{};
    for (auto const &p : processes) {
        average_turnaround_time += p.runtime_info.turnaround;
        average_weighed_turnaround_time += p.runtime_info.weighed_turnaround;
    }
    average_turnaround_time /= static_cast<double>(processes.size());
    average_weighed_turnaround_time /= static_cast<double>(processes.size());

    fast_io::println(
        "Average turnaround time of the system: ",
        std::string(
            offsets[index_of_item.at("Turnaround time")] -
                fast_io::string_view("Average turnaround time of the system: ")
                    .size(),
            ' '),
        fast_io_ext::mnp::fixed(average_turnaround_time, 2));
    fast_io::println(
        "Average weighed turnaround time of the system: ",
        std::string(offsets[index_of_item.at("Weighed turnaround time")] -
                        fast_io::string_view(
                            "Average weighed turnaround time of the system: ")
                            .size(),
                    ' '),
        fast_io_ext::mnp::fixed(average_weighed_turnaround_time, 2));
}
