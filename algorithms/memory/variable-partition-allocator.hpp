#pragma once

#include <algorithms/memory/allocator.hpp>
#include <algorithms/memory/block.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <list>
#include <print>
#include <ranges>

namespace shelpam::memory {

struct First_fit_policy {
    auto operator()(std::ranges::range auto &&iters) const
    {
        return iters.front();
    }
};
struct Best_fit_policy {
    auto operator()(std::ranges::range auto &&iters) const
    {
        return std::ranges::min(iters, {}, [](auto it) { return it->size; });
    }
};
struct Worst_fit_policy {
    auto operator()(std::ranges::range auto &&iters) const
    {
        return std::ranges::max(iters, {}, [](auto it) { return (*it).size; });
    }
};

class Variable_partition_allocator {
  public:
    friend std::string to_string(Variable_partition_allocator const &a)
    {
        return to_string(a.blocks_);
    }

    Variable_partition_allocator(Block block) : blocks_{std::move(block)} {}

    // Requirement:
    //   Given Policy p, p(blocks) should produce wanted iterator to the block.
    template <typename Policy = First_fit_policy>
    void *allocate(std::string name, std::size_t size, Policy &&policy = {})
    {
        static_assert(sizeof(char) == 1,
                      "The code depends on char size being 1.");

        std::vector<decltype(blocks_)::iterator> good;
        for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
            if (!it->owner.has_value() && it->size >= size) {
                good.push_back(it);
            }
        }
        if (good.empty()) {
            return nullptr;
        }

        auto it = std::forward<Policy>(policy)(good);
        if (it->size > size) {
            blocks_.insert(std::next(it),
                           {.address = static_cast<char *>(it->address) + size,
                            .size = it->size - size});
        }
        it->size = size;
        it->owner = std::move(name);

        return it->address;
    }

    void deallocate(void *ptr)
    {
        static_assert(sizeof(char) == 1,
                      "The code depends on char size being 1.");

        auto it = std::ranges::find(blocks_, ptr, &Block::address);
        if (it == blocks_.end()) {
            throw std::runtime_error(
                std::format("memory block (starting at {}) wasn't "
                            "allocated by this allocator",
                            ptr));
        }
        it->owner.reset();

        merge_free_blocks(it);
        if (it != blocks_.begin()) { // if it has element before it
            merge_free_blocks(std::prev(it));
        }
    }

  private:
    std::list<Block> blocks_;

    // Merges it and it + 1 if they are both free.
    // Params:
    //  it  should be valid (dereferenceable).
    void merge_free_blocks(decltype(blocks_)::iterator it)
    {
        // Doesn't have next block
        if (std::next(it) == blocks_.end()) {
            return;
        }

        // Any of the block are not free
        if (it->owner.has_value() || std::next(it)->owner.has_value()) {
            return;
        }

        it->size += std::next(it)->size;
        blocks_.erase(std::next(it));
    }
};

} // namespace shelpam::memory
