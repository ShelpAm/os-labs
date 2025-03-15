#include <process/process.hpp>

#include <nlohmann/json.hpp>

// FROM AI, because I'm lack of UTF-8 knowledges...
// Function to count Chinese characters in a UTF-8 encoded std::string
std::size_t count_chinese_characters(std::string_view str)
{
    size_t chinese_count = 0;
    size_t i = 0;

    while (i < str.size()) {
        // Determine the number of bytes in the current UTF-8 character
        auto first_byte = static_cast<uint8_t>(str[i]);
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
            code_point |= static_cast<uint8_t>(str[i + j]) & 0x3F;
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

fast_io::vector<Process> input_processes()
{
    fast_io::print("Please input the number of processes: ");
    int num_processes{};
    fast_io::scan(num_processes);

    fast_io::vector<Process> jobs;
    for (int i{}; i != num_processes; ++i) {
        fast_io::println("Please input info of the ", i + 1,
                         "-th process (in an order of id, name, arriving time, "
                         "and execution time):");
        jobs.push_back(Process(fast_io::c_stdin()));
    }

    { // Load priorities from ./tests/config.json
        fast_io::native_file_loader f("./tests/config.json");
        fast_io::string_view s(std::from_range, f);
        auto const j(nlohmann::json::parse(s));
        auto const priorities{j["priorities"].get<fast_io::vector<int>>()};
        for (auto [job, priority] : std::views::zip(jobs, priorities)) {
            job.set_priority(priority);
        }

        // auto &&cao{j.at("jobs").get<fast_io::vector<Process>>()};
        // output_processes_info(cao);

        // output_processes_info(j.at("jobs").get<fast_io::vector<Process>>());

        // auto v = fast_io::vector<Process>();
        // output_processes_info(v);
        // output_processes_info(fast_io::vector<Process>());
    }

    std::ranges::sort(jobs, {}, &Process::arrival_time);

    return jobs;
}
