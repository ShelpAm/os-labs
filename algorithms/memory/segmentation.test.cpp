#include <algorithms/memory/segmentation.hpp>
#include <gtest/gtest.h>

namespace shelpam::memory {

TEST(SegmentationAllocator, Basics)
{
    Segmentation seg(Block{.address = reinterpret_cast<void *>(40),
                           .size = 256,
                           .owner = std::nullopt});
    seg.summary();
    seg.allocate("Job A", {20, 30});
    seg.summary();
    seg.allocate("Job B", {30, 35, 35});
    seg.summary();
    seg.deallocate("Job B");
    seg.summary();
    seg.deallocate("Job A");
    seg.summary();
}

} // namespace shelpam::memory

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
