#include <algorithms/memory/variable-partition-allocator.hpp>
#include <iostream>
#include <map>
#include <print>
#include <string>
#include <unordered_map>
#include <vector>

namespace shelpam::memory {

class Segmentation {
  public:
    struct Segment_table {
        std::vector<void *> bases;
    };

    Segmentation(Block initial_block) : allocator_(std::move(initial_block)) {}

    template <typename Policy = First_fit_policy>
    void allocate(std::string job, std::vector<std::size_t> const &sizes,
                  Policy &&policy = {})
    {
        Segment_table st;
        for (auto size : sizes) {
            st.bases.push_back(
                allocator_.allocate(job, size, std::forward<Policy>(policy)));
        }
        segment_tables_.insert({std::move(job), std::move(st)});
    }

    void deallocate(std::string const &job)
    {
        auto it = segment_tables_.find(job);
        if (it == segment_tables_.end()) {
            throw std::runtime_error("job does't exist");
        }

        for (void *base : it->second.bases) {
            allocator_.deallocate(base);
        }

        segment_tables_.extract(it);
    }

    void summary(std::ostream &os = std::cout)
    {
        std::println(os, "Segmentation summary");
        std::println(os, "\tSegment tables");
        auto sorted_segment_table = std::map(std::from_range, segment_tables_);
        for (auto const &[job, st] : sorted_segment_table) {
            std::println(os, "\t\tJob {} bases", job);
            for (void *base : st.bases) {
                std::println(os, "\t\t\t{}", base);
            }
        }
        auto allocator_summary = to_string(allocator_);
        std::string patched_allocator_summary;
        for (auto line : allocator_summary | std::views::split('\n')) {
            patched_allocator_summary +=
                "\t\t" + std::string(std::string_view(line)) + '\n';
        }
        std::println(os, "\tAllocator summary\n{}", patched_allocator_summary);
        std::println(os, "");
    }

  private:
    Variable_partition_allocator allocator_;
    std::unordered_map<std::string, Segment_table> segment_tables_;
};

} // namespace shelpam::memory
