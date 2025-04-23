#include <algorithms/memory/page-allocator.hpp>
#include <gtest/gtest.h>

namespace shelpam::memory::page {

TEST(PageAllocator, Basics)
{
    Page_allocator alloc(1 << 10 | 1 << 8, 4);
    auto pt = alloc.allocate("Job A", 1 << 4);
    pt = alloc.allocate("Job B", 1 << 6);
    alloc.deallocate("Job A");
    alloc.print();
    pt = alloc.allocate("Job C", 1 << 8);
    alloc.print();
}

} // namespace shelpam::memory::page

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
