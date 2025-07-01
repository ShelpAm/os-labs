#include <cstddef>
#include <deque>
#include <print>
#include <queue>
#include <unordered_set>

namespace shelpam::memory {

// Memory accessing process:
//
// Access(page)
//    if page in memory pages
//        update its parameter
//    else
//        raise page fault

using Page = std::size_t;

class Page_replacement_policy {
  public:
    Page_replacement_policy(Page max_pages) : max_pages_{max_pages} {}
    Page_replacement_policy(Page_replacement_policy const &) = default;
    Page_replacement_policy(Page_replacement_policy &&) = delete;
    Page_replacement_policy &
    operator=(Page_replacement_policy const &) = default;
    Page_replacement_policy &operator=(Page_replacement_policy &&) = delete;
    virtual ~Page_replacement_policy() = default;

    void access(Page page)
    {
        std::println("accessing page {}", page);
        if (!is_in_memory(page)) {
            std::println("page {} misses, raising page fault", page);
            handle_page_fault(page);
            ++miss_times_;
        }
        else {
            std::println("page {} hits", page);
        }

        load_page(page);
        ++access_times_;
    }

    [[nodiscard]] double miss_ratio() const
    {
        return static_cast<double>(miss_times_) /
               static_cast<double>(access_times_);
    }

    [[nodiscard]] std::size_t max_pages() const
    {
        return max_pages_;
    }

  private:
    std::size_t max_pages_;
    std::size_t access_times_{};
    std::size_t miss_times_{};

    virtual void handle_page_fault(Page page) = 0;
    virtual void load_page(Page page) = 0;
    [[nodiscard]] virtual bool is_in_memory(Page page) const = 0;
};

// Least recent used
// class LRU : public Page_replacement_policy {
//   public:
//     LRU() = default;
//     LRU(const LRU &) = default;
//     LRU(LRU &&) = delete;
//     LRU &operator=(const LRU &) = default;
//     LRU &operator=(LRU &&) = delete;
//     LRU(std::size_t num_pages) : in_use_(num_pages) {}
//     virtual ~LRU() = default;
//     void access(std::size_t page)
//     {
//         if (in_use_[page]) {
//             handle_page_fault(page);
//         }
//
//         // Do access the memory
//     }
//
//   private:
//     std::deque<bool> in_use_;
//
//     void handle_page_fault(std::size_t page)
//     {
//         // Try to find free page
//         // auto it = find_free_page();
//         // if (it == nullptr) {
//         // }
//     }
// };

// First in first out
class FIFO : public Page_replacement_policy {
  public:
    FIFO(Page max_pages) : Page_replacement_policy(max_pages) {}
    FIFO(const FIFO &) = default;
    FIFO(FIFO &&) = delete;
    FIFO &operator=(const FIFO &) = default;
    FIFO &operator=(FIFO &&) = delete;
    ~FIFO() override = default;

  private:
    std::deque<Page> pages_queue_;
    std::unordered_set<Page> pages_set_;

    void handle_page_fault(Page page) override
    {
        std::println("max pages: {}", max_pages());
        if (pages_queue_.size() == max_pages()) {
            pages_set_.erase(pages_queue_.front());
            pages_queue_.pop_front();
        }
    }

    void load_page(Page page) override
    {
        pages_set_.insert(page);
        pages_queue_.push_back(page);
        std::println("queue: ");
        for (auto page : pages_queue_) {
            std::print("{} ", page);
        }
        std::println("");
    }

    [[nodiscard]] bool is_in_memory(Page page) const override
    {
        return pages_set_.contains(page);
    }
};

} // namespace shelpam::memory
