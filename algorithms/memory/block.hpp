#pragma once

#include <cstddef>
#include <format>
#include <span>
#include <sstream>
#include <string>
#include <string_view>

namespace shelpam::memory {

struct Block {
    void *address;
    std::size_t size;
    std::optional<std::string> owner;
};

std::string to_string(std::ranges::range auto blocks)
{
    static_assert(
        std::same_as<Block, std::ranges::range_value_t<decltype(blocks)>>,
        "blocks should be a range of Block");

    std::ostringstream oss;
    oss << std::format("Address   Size   Occupied by   \n");
    for (auto const &blk : blocks) {
        oss << std::format("{:^7}   {:^4}   {:^11}\n",
                           std::bit_cast<std::size_t>(blk.address), blk.size,
                           blk.owner.value_or("-"));
    }
    return oss.str();
}

} // namespace shelpam::memory
