#include <algorithms/memory/variable-partition-allocator.hpp>
#include <gtest/gtest.h>

namespace shelpam::memory {

TEST(FixedSizeAllocator, Basics)
{
    {
        First_fit_policy policy;
        Variable_partition_allocator alloc(
            Block{.address = reinterpret_cast<void *>(40), .size = 256});

        auto *a = alloc.allocate("Job A", 15, policy);
        auto *b = alloc.allocate("Job B", 50, policy);
        auto *c = alloc.allocate("Job C", 10, policy);
        auto *d = alloc.allocate("Job D", 25, policy);
        auto *e = alloc.allocate("Job E", 14, policy);

        std::println("{}", to_string(alloc));

        alloc.deallocate(b);
        alloc.deallocate(d);

        auto *f = alloc.allocate("Job F", 32);

        std::println("{}", to_string(alloc));
    }

    {
        Best_fit_policy policy;
        Variable_partition_allocator alloc(
            Block{.address = reinterpret_cast<void *>(40), .size = 256});

        auto *a = alloc.allocate("Job A", 15, policy);
        auto *b = alloc.allocate("Job B", 50, policy);
        auto *c = alloc.allocate("Job C", 10, policy);
        auto *d = alloc.allocate("Job D", 25, policy);
        auto *e = alloc.allocate("Job E", 14, policy);

        std::println("{}", to_string(alloc));

        alloc.deallocate(b);
        alloc.deallocate(d);

        auto *f = alloc.allocate("Job F", 32);

        std::println("{}", to_string(alloc));
    }

    {
        Worst_fit_policy policy;
        Variable_partition_allocator alloc(
            Block{.address = reinterpret_cast<void *>(40), .size = 256});

        auto *a = alloc.allocate("Job A", 15, policy);
        auto *b = alloc.allocate("Job B", 50, policy);
        auto *c = alloc.allocate("Job C", 10, policy);
        auto *d = alloc.allocate("Job D", 25, policy);
        auto *e = alloc.allocate("Job E", 14, policy);

        std::println("{}", to_string(alloc));

        alloc.deallocate(b);
        alloc.deallocate(d);

        auto *f = alloc.allocate("Job F", 32);

        std::println("{}", to_string(alloc));
    }
}

} // namespace shelpam::memory

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
