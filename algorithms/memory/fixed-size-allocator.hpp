#pragma once

#include <algorithms/memory/allocator.hpp>
#include <algorithms/memory/block.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <format>
#include <list>
#include <print>
#include <sstream>
#include <string>
#include <unordered_map>

namespace shelpam::memory {

class Fixed_size_allocator {
  public:
    friend std::string to_string(Fixed_size_allocator const &a)
    {
        return to_string(a.blocks_);
    }

    Fixed_size_allocator(auto blocks)
        : blocks_(std::from_range, std::move(blocks))
    {
        for (auto &blk : blocks_) {
            block_of_mem_addr[blk.address] = &blk;
        }
    }

    void *allocate(std::string const &name, std::size_t size)
    {
        auto it = std::ranges::find_if(blocks_, [size](Block const &blk) {
            return !blk.owner.has_value() && blk.size >= size;
        });
        if (it == blocks_.end()) {
            return nullptr;
        }
        it->owner = name;
        std::println("After allocation to {}, state is:\n{}", name,
                     to_string(*this));
        return it->address;
    }

    void deallocate(void *ptr)
    {
        auto it = block_of_mem_addr.find(ptr);
        assert(it != block_of_mem_addr.end());

        Block *blk = it->second;
        auto owner = std::move(blk->owner);
        blk->owner.reset();
        std::println("After deallocation from {}, state is:\n{}", owner.value(),
                     to_string(*this));
        block_of_mem_addr.erase(blk);
    }

  private:
    std::list<Block> blocks_;
    std::unordered_map<void *, Block *> block_of_mem_addr;
};

} // namespace shelpam::memory
