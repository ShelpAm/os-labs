#include <algorithms/memory/page-allocator.hpp>
#include <algorithms/memory/page-replacement.hpp>
#include <gtest/gtest.h>

namespace shelpam::memory {

TEST(PageReplacement, Basics)
{
#if 0
    page::Page_allocator pa;
    Page_replacement_policy *const policy = new FIFO(3);
    std::vector<std::size_t> const pages{7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
                                         0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    for (auto page : pages) {
        // policy->access(page);
        access(page);
    }

    EXPECT_DOUBLE_EQ(policy->miss_ratio(), 0.75);
#endif
}

} // namespace shelpam::memory

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
