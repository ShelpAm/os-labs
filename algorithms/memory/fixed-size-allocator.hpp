#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <format>
#include <list>
#include <print>
#include <sstream>
#include <string>
#include <unordered_map>

namespace shelpam::allocators {

class Fixed_size_allocator {
  public:
    struct Block {
        static constexpr std::string_view free_status{"-"};
        std::size_t id;
        std::size_t size;
        void *address;
        std::string status{free_status};
    };

    friend std::string to_string(Fixed_size_allocator const &a)
    {
        std::ostringstream oss;
        oss << std::format("ID   Size   Address   Occupied by   \n");
        for (auto const &blk : a.blocks_) {
            oss << std::format("{:^2}   {:^4}   {:^7}   {:^11}\n", blk.id,
                               blk.size, blk.address, blk.status);
        }
        return oss.str();
    }

    Fixed_size_allocator(auto blocks)
        : blocks_(std::from_range, std::move(blocks))
    {
        for (auto &blk : blocks_) {
            block_of_mem_addr[blk.address] = &blk;
        }
    }

    // Return:
    //  nullptr  if no available space
    void *allocate(std::string const &name, std::size_t size)
    {
        auto it = std::ranges::find_if(blocks_, [size](Block const &blk) {
            return blk.status == Block::free_status && blk.size >= size;
        });
        if (it == blocks_.end()) {
            return nullptr;
        }
        it->status = name;
        std::println("After allocation to {}, state is:\n{}", name,
                     to_string(*this));
        return it->address;
    }

    void deallocate(void *address)
    {
        auto it = block_of_mem_addr.find(address);
        assert(it != block_of_mem_addr.end());

        Block *blk = it->second;
        auto status = std::move(blk->status);
        blk->status = Block::free_status;
        std::println("After deallocation from {}, state is:\n{}", status,
                     to_string(*this));
        block_of_mem_addr.erase(blk);
    }

  private:
    std::list<Block> blocks_;
    std::unordered_map<void *, Block *> block_of_mem_addr;
};

} // namespace shelpam::allocators
