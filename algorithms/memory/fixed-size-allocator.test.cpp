#include <algorithms/memory/fixed-size-allocator.hpp>
#include <gtest/gtest.h>

namespace shelpam::allocators {

TEST(FixedSizeAllocator, Basics)
{
    std::vector<Fixed_size_allocator::Block> const blocks{
        {.id = 1, .size = 12, .address = reinterpret_cast<void *>(20)},
        {.id = 2, .size = 32, .address = reinterpret_cast<void *>(32)},
        {.id = 3, .size = 64, .address = reinterpret_cast<void *>(64)},
        {.id = 4, .size = 128, .address = reinterpret_cast<void *>(128)},
        {.id = 5, .size = 100, .address = reinterpret_cast<void *>(256)},
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

} // namespace shelpam::allocators

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
