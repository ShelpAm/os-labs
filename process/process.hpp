#pragma once

#include <process/cpu.hpp>
#include <process/time.hpp>

#include <compare>
#include <fast-io-ext/fixed.hpp>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
#include <map>
#include <stdexcept>

struct Process {
    explicit constexpr Process(fast_io::c_io_observer in)
    {
        int id;
        fast_io::string name;
        fast_io::string arriving_time;
        int execution_time;

        fast_io::scan(in, id, name, arriving_time, execution_time);

        this->id = id;
        this->name = name;
        this->arriving_time = arriving_time;
        this->execution_time = execution_time;

        this->remaining_execution_time = execution_time;
    }

    constexpr Process(Process const &) = default;
    constexpr Process(Process &&) = default;
    Process &operator=(Process const &) = default;
    Process &operator=(Process &&) = default;
    ~Process() = default;

    constexpr auto operator<=>(Process const &rhs) const noexcept = default;

    [[nodiscard]] constexpr bool started() const
    {
        return remaining_execution_time != execution_time;
    }

    [[nodiscard]] constexpr bool finished() const
    {
        return remaining_execution_time == 0;
    }

    constexpr void run_for(int minutes, CPU const &cpu)
    {
        if (finished()) {
            throw std::logic_error(
                "cannot run this process, it has been finished");
        }

        if (!started()) {
            start_time = cpu.now();
        }

        // Running time shouldn't exceed remaining execution time.
        minutes = std::min(minutes, remaining_execution_time);

        remaining_execution_time -= minutes;

        if (finished()) {
            finish_time = cpu.now() + minutes;
            calculate_process_stats();
        }
    }

    // Intrinsic properties
    int id;
    fast_io::string name;
    Time arriving_time;
    int execution_time;

    // Non-intrinsic properties
    Time start_time;
    Time finish_time;
    int turnaround_time;            // 周转时间
    double weighed_turnaround_time; // 带权周转时间

    // Runtime info
    int remaining_execution_time;

  private:
    constexpr void calculate_process_stats()
    {
        turnaround_time = finish_time - arriving_time;
        weighed_turnaround_time =
            static_cast<double>(turnaround_time) / execution_time;
    }
};

std::size_t count_chinese_characters(fast_io::string const &str);

void output_processes_info(std::ranges::range auto &&processes)
{
    fast_io::println("Simulation result:");
    fast_io::vector<fast_io::string_view> items{"ID",
                                                "Name",
                                                "Arriving time",
                                                "Execution time",
                                                "Start time",
                                                "Finish time",
                                                "Turnaround time",
                                                "Weighed turnaround time"};
    std::string const padding(10, ' ');
    fast_io::vector<std::size_t> offsets;
    std::map<fast_io::string, std::size_t> offset_table;
    // item1<3 spaces>item2<3 spaces>...
    fast_io::string heading;
    for (std::size_t offset{}; auto const &item : items) {
        fast_io::string part{item};
        part.append(fast_io::mnp::os_c_str(padding.c_str()));
        heading.append(part);
        offsets.push_back(offset);
        offset_table[fast_io::string(item)] = offset;
        offset += part.size();
    }
    fast_io::println(heading);
    for (auto const &p : processes) {
        fast_io::string buffer;
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

        // Build the line
        align_next();
        buffer.append(fast_io::to<fast_io::string>(p->id));
        align_next();
        buffer.append(p->name);
        offset_for_chinese +=
            -static_cast<int>(count_chinese_characters(p->name));
        align_next();
        buffer.append(to_string(p->arriving_time));
        align_next();
        buffer.append(fast_io::to<fast_io::string>(p->execution_time));
        align_next();
        buffer.append(to_string(p->start_time));
        align_next();
        buffer.append(to_string(p->finish_time));
        align_next();
        buffer.append(fast_io::to<fast_io::string>(p->turnaround_time));
        align_next();
        buffer.append(fast_io_ext::mnp::fixed(p->weighed_turnaround_time, 2));

        fast_io::println(buffer);
    }

    double average_turnaround_time{};
    double average_weighed_turnaround_time{};
    for (auto const &p : processes) {
        average_turnaround_time += p->turnaround_time;
        average_weighed_turnaround_time += p->weighed_turnaround_time;
    }
    average_turnaround_time /= static_cast<double>(processes.size());
    average_weighed_turnaround_time /= static_cast<double>(processes.size());

    fast_io::println(
        "Average turnaround time of the system: ",
        std::string(
            offset_table[fast_io::string("Turnaround time")] -
                fast_io::string_view("Average turnaround time of the system: ")
                    .size(),
            ' '),
        average_turnaround_time);
    // std::printf("%.2f\n", average_turnaround_time);
    fast_io::println(
        "Average weighed turnaround time of the system: ",
        std::string(offset_table[fast_io::string("Weighed turnaround time")] -
                        fast_io::string_view(
                            "Average weighed turnaround time of the system: ")
                            .size(),
                    ' '),
        fast_io_ext::mnp::fixed(average_weighed_turnaround_time, 2));
}
