#include <algorithms/memory/fixed-size-allocator.hpp>
#include <gtest/gtest.h>

namespace shelpam::memory {

TEST(FixedSizeAllocator, Basics)
{
    std::vector<Block> const blocks{
        {.address = reinterpret_cast<void *>(20), .size = 12},
        {.address = reinterpret_cast<void *>(32), .size = 32},
        {.address = reinterpret_cast<void *>(64), .size = 64},
        {.address = reinterpret_cast<void *>(128), .size = 128},
        {.address = reinterpret_cast<void *>(256), .size = 100},
    };

    Fixed_size_allocator allocator(blocks);

    std::vector<std::size_t> release_order{1, 2, 0};
    std::vector<void *> addresses;

    addresses.push_back(allocator.allocate("job1", 30));
    addresses.push_back(allocator.allocate("job2", 60));
    addresses.push_back(allocator.allocate("job3", 90));

    for (auto idx : release_order) {
        allocator.deallocate(addresses[idx]);
    }
}

} // namespace shelpam::memory

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
