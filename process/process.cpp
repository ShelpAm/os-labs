#include <process/process.hpp>

// FROM AI, because I'm lack of UTF-8 knowledges...
// Function to count Chinese characters in a UTF-8 encoded std::string
std::size_t count_chinese_characters(fast_io::string const &str)
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
