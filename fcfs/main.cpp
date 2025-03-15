#include <algorithm>
#include <fast-io-ext/fixed.hpp>
#include <fast_io.h>
#include <map>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

// FROM AI, because I'm lack of UTF-8 knowledges...
// Function to count Chinese characters in a UTF-8 encoded std::string
std::size_t count_chinese_characters(std::string_view s)
{
    size_t chinese_count = 0;
    size_t i = 0;

    while (i < s.size()) {
        // Determine the number of bytes in the current UTF-8 character
        auto first_byte = static_cast<uint8_t>(s[i]);
        size_t char_len = 0;

        if ((first_byte & 0x80) == 0) { // 1-byte character (ASCII)
            char_len = 1;
        }
        else if ((first_byte & 0xE0) == 0xC0) { // 2-byte character
            char_len = 2;
        }
        else if ((first_byte & 0xF0) == 0xE0) { // 3-byte character
            char_len = 3;
        }
        else if ((first_byte & 0xF8) == 0xF0) { // 4-byte character
            char_len = 4;
        }
        else {
            // Invalid UTF-8 sequence
            ++i;
            continue;
        }

        // Extract the Unicode code point
        uint32_t code_point = 0;
        for (size_t j = 0; j < char_len; ++j) {
            code_point <<= 6;
            code_point |= static_cast<uint8_t>(s[i + j]) & 0x3F;
        }

        // Check if the code point is a Chinese character
        if ((code_point >= 0x4E00 &&
             code_point <= 0x9FFF) || // CJK Unified Ideographs
            (code_point >= 0x3400 && code_point <= 0x4DBF) ||   // Extension A
            (code_point >= 0x20000 && code_point <= 0x2A6DF) || // Extension B
            (code_point >= 0x2A700 && code_point <= 0x2B73F) || // Extension C
            (code_point >= 0x2B740 && code_point <= 0x2B81F)) { // Extension D
            ++chinese_count;
        }

        // Move to the next character
        i += char_len;
    }

    return chinese_count;
}

constexpr int to_int(std::string_view s)
{
    constexpr auto base{10};
    int result{};
    for (auto const c : s) {
        result = result * base + (c - '0');
    }
    return result;
}

struct Time {
    constexpr Time() = default;

    constexpr explicit Time(std::string const &s)
    {
        auto const p{s.find(':')};
        auto const hour{to_int(s.substr(0, p))};
        auto const minute{to_int(s.substr(p + 1))};
        this->minutes_ = hour * minutes_per_hour + minute;
    }

    constexpr Time &operator=(std::string const &s)
    {
        return *this = Time(s);
    }

    constexpr auto operator<=>(Time const &rhs) const noexcept = default;

    constexpr Time &operator+=(int delta_minutes)
    {
        minutes_ += delta_minutes;
        return *this;
    }

    friend constexpr Time operator+(Time lhs, int delta_minutes)
    {
        return lhs += delta_minutes;
    }

    friend constexpr int operator-(Time const &lhs, Time const &rhs)
    {
        return lhs.minutes_ - rhs.minutes_;
    }

    static constexpr auto minutes_per_hour{60};
    int minutes_;
};

constexpr fast_io::string to_string(Time const &t)
{
    auto hour{
        fast_io::to<fast_io::string>(t.minutes_ / Time::minutes_per_hour)};
    auto minute{
        fast_io::to<fast_io::string>(t.minutes_ % Time::minutes_per_hour)};
    if (hour.size() == 1) {
        hour = fast_io::concat_fast_io(fast_io::mnp::chvw('0'), hour);
    }
    if (minute.size() == 1) {
        minute = fast_io::concat_fast_io(fast_io::mnp::chvw('0'), minute);
    }
    return fast_io::concat_fast_io(hour, fast_io::mnp::chvw(':'), minute);
}

struct Process {
    constexpr auto operator<=>(Process const &rhs) const noexcept = default;

    constexpr void start_at(Time started_time)
    {
        start_time = started_time;
        finish_time = start_time + total_execution_time;
        turnaround_time = finish_time - arrival_time;
        weighed_turnaround_time =
            static_cast<double>(turnaround_time) / total_execution_time;
    }

    // Intrinsic properties
    int id;
    std::string name;
    Time arrival_time;
    int total_execution_time;

    // Non-intrinsic properties
    Time start_time;
    Time finish_time;
    int turnaround_time;            // 周转时间
    double weighed_turnaround_time; // 带权周转时间
};

int main()
{
    // try {
    fast_io::print("Please input the number of processes: ");
    int num_processes{};
    fast_io::scan(num_processes);

    std::vector<Process> processes;
    for (int i{}; i != num_processes; ++i) {
        fast_io::println("Please input info of the ", i + 1,
                         "-th process (in an order of id, name, arriving time, "
                         "and consumed time): ");

        int id;
        std::string name;
        std::string arriving_time;
        int execution_time;
        fast_io::scan(id, name, arriving_time, execution_time);
        Process p{.id = id,
                  .name{name},
                  .arrival_time{arriving_time},
                  .total_execution_time = execution_time};
        processes.push_back(p);
    }

    std::ranges::sort(processes, {}, &Process::arrival_time);

    for (Time now{}; auto &p : processes) {
        p.start_at(now < p.arrival_time ? p.arrival_time : now);
        now = p.finish_time;
    }

    fast_io::println("Simulation result:");
    std::vector<std::string_view> items{"ID",
                                        "Name",
                                        "Arriving time",
                                        "Execution time",
                                        "Start time",
                                        "Finish time",
                                        "Turnaround time",
                                        "Weighed turnaround time"};
    std::string const padding(10, ' ');
    std::vector<std::size_t> offsets;
    std::map<std::string, std::size_t> offset_table;
    // item1<3 spaces>item2<3 spaces>...
    std::string heading;
    for (std::size_t offset{}; auto const &item : items) {
        std::string part{item};
        part.append(padding);
        heading += part;
        offsets.push_back(offset);
        offset_table[std::string(item)] = offset;
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
        buffer.append(fast_io::to<fast_io::string>(p.id));
        align_next();
        buffer.append(fast_io::to<fast_io::string>(p.name));
        offset_for_chinese +=
            -static_cast<int>(count_chinese_characters(p.name));
        align_next();
        buffer.append(to_string(p.arrival_time));
        align_next();
        buffer.append(fast_io::to<fast_io::string>(p.total_execution_time));
        align_next();
        buffer.append(to_string(p.start_time));
        align_next();
        buffer.append(to_string(p.finish_time));
        align_next();
        buffer.append(fast_io::to<fast_io::string>(p.turnaround_time));
        align_next();
        buffer.append(fast_io_ext::mnp::fixed(p.weighed_turnaround_time, 2));

        fast_io::println(buffer);
    }

    double average_turnaround_time{};
    double average_weighed_turnaround_time{};
    for (auto const &p : processes) {
        average_turnaround_time += p.turnaround_time;
        average_weighed_turnaround_time += p.weighed_turnaround_time;
    }
    average_turnaround_time /= static_cast<double>(processes.size());
    average_weighed_turnaround_time /= static_cast<double>(processes.size());

    fast_io::println(
        "Average turnaround time of the system: ",
        std::string(
            offset_table[std::string("Turnaround time")] -
                std::string_view("Average turnaround time of the system: ")
                    .size(),
            ' '),
        average_turnaround_time);
    // std::printf("%.2f\n", average_turnaround_time);
    fast_io::println(
        "Average weighed turnaround time of the system: ",
        std::string(offset_table[std::string("Weighed turnaround time")] -
                        std::string_view(
                            "Average weighed turnaround time of the system: ")
                            .size(),
                    ' '),
        fast_io_ext::mnp::fixed(average_weighed_turnaround_time, 2));
    // fast_io::to<fast_io::string>(average_weighed_turnaround_time)
    //     .substr(0, 4));
    // std::printf("%.2f\n", average_weighed_turnaround_time);
    // }
    // catch (fast_io::error const &e) {
    //     fast_io::println(e);
    //     throw;
    // }
}
