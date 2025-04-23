#pragma once

#include <cassert>
#include <climits>
#include <iostream>
#include <print>
#include <ranges>
#include <unordered_map>
#include <vector>

namespace shelpam::memory::page {

using Page_table = std::vector<std::size_t>;

class Page_allocator {
    friend class Memory_iterator;
    using Word = std::uint_least64_t;
    static constexpr std::size_t word_size{sizeof(Word)};
    static constexpr auto word_bit = word_size * CHAR_BIT;

  public:
    Page_allocator(std::size_t memory_byte_size, std::size_t page_size)
        : page_size_{page_size}, total_pages_{least_pages(memory_byte_size)},
          available_pages_{total_pages_},
          allotment_((total_pages_ + word_bit - 1) / word_bit)
    {
    }

    Page_table const &allocate(std::string job, std::size_t size)
    {
        if (page_tables_.contains(job)) {
            throw std::runtime_error(
                "last allocated memory for job hasn't been released");
        }

        auto pages_needed = least_pages(size);

        if (available_pages_ < pages_needed) {
            throw std::runtime_error("no enough page to allocate");
        }

        std::size_t i{};
        auto next_free_page = [this, &i] -> std::size_t {
            while (i != total_pages_) {
                auto word = i / word_bit;
                auto bit = i % word_bit;
                if ((allotment_[word] & 1ULL << bit) == 0) {
                    allotment_[word] |= 1ULL << bit;
                    return (word * word_bit) + bit;
                }
                ++i;
            }
            std::unreachable();
        };

        Page_table pt;
        while (pages_needed != 0) {
            pt.push_back(next_free_page());
            --available_pages_;
            --pages_needed;
        }
        auto &start =
            page_tables_.insert({std::move(job), std::move(pt)}).first->second;
        return start;
    }

    void deallocate(std::string const &job)
    {
        if (!job.contains(job)) {
            throw std::runtime_error("job doesn't exist");
        }

        auto it = page_tables_.find(job);
        auto &pt = it->second;
        for (auto p : pt) {
            auto word = p / word_bit;
            auto bit = p % word_bit;
            allotment_[word] ^= 1ULL << bit;
            ++available_pages_;
        }
        page_tables_.extract(it);
    }

    void print(std::ostream &os = std::cout)
    {
        std::println(os, "Bit map:");
        for (auto word : allotment_) {
            for (int i{}; i != word_bit; ++i) {
                std::print(os, "{}", word >> i & 1);
            }
            std::println(os);
        }
        std::println(os, "Page tables:");
        for (auto const &[job, table] : page_tables_) {
            std::println(os, "\tJob '{}', page table:", job);
            for (int i{}; auto e : table) {
                std::println(os, "\t\t{}  {}", i, e);
                ++i;
            }
        }
    }

  private:
    std::size_t page_size_;
    std::size_t total_pages_;
    std::size_t available_pages_; // Available number of pages
    std::vector<Word> allotment_;
    std::unordered_map<std::string, Page_table> page_tables_;

    [[nodiscard]] std::size_t least_pages(std::size_t size_bytes) const
    {
        return (size_bytes + page_size_ + 1) / page_size_;
    }
};

} // namespace shelpam::memory::page
