#include <algorithms/disk-arm-scheduling.hpp>
#include <gtest/gtest.h>

namespace shelpam::disk_arm_scheduling {

void print_seek_path(std::span<Position const> requests,
                     std::span<std::size_t const> order, Position min_cylinder,
                     Position max_cylinder, int canvas_width = 80)
{
    auto scale = [&](Position pos) -> int {
        return (pos - min_cylinder) * canvas_width /
               (max_cylinder - min_cylinder + 1);
    };

    std::println("Seek Path Visualization:");

    std::string line(canvas_width, ' ');

    for (std::size_t i = 0; i < order.size(); ++i) {
        int index = order[i];
        Position request = requests[index];
        int pos = scale(request);

        if (i == 0) {
            line[pos] = '*';
        }
        else {
            int prev_pos = scale(requests[order[i - 1]]);
            int from = std::min(prev_pos, pos);
            int to = std::max(prev_pos, pos);

            for (int j = from + 1; j < to; ++j)
                line[j] = '-';

            if (prev_pos < pos)
                line[prev_pos] = '>';
            else
                line[prev_pos] = '<';

            line[pos] = '*';
        }

        std::println("{:>2}: {} ({})", i, line, request);
        line.assign(canvas_width, ' '); // Reset line
    }

    std::string idx_str;
    for (auto i : order) {
        idx_str += std::format("{} ", i);
    }
}

class DiskArmSchedulingTest : public ::testing::Test {
  protected:
    Context const ctx{
        .min_cylinder = 0,
        .max_cylinder = 199,
        .current_position = 125,
    };
    std::vector<int> const requests{86, 147, 91, 177, 94, 150, 102, 175, 130};
};

TEST_F(DiskArmSchedulingTest, FCFS)
{
    auto res = fcfs(ctx, requests);
    std::println("FCFS: {}", res.total_distance);
    std::println("FCFS order: {}", res.order);
    print_seek_path(requests, res.order, ctx.min_cylinder, ctx.max_cylinder);
}

TEST_F(DiskArmSchedulingTest, SSTF)
{
    auto res = sstf(ctx, requests);
    std::println("SSTF total_distance: {}", res.total_distance);
    std::println("SSTF order: {}", res.order);
    print_seek_path(requests, res.order, ctx.min_cylinder, ctx.max_cylinder);
}

TEST_F(DiskArmSchedulingTest, ElevatorDown)
{
    auto res = elevator(ctx, requests, Direction::down);
    std::println("Elevator Down initial direction: down");
    std::println("total_distance: {}", res.total_distance);
    std::println("order: {}", res.order);
    print_seek_path(requests, res.order, ctx.min_cylinder, ctx.max_cylinder);
}

TEST_F(DiskArmSchedulingTest, ElevatorUp)
{
    auto res = elevator(ctx, requests, Direction::up);
    std::println("Elevator Up initial direction: up");
    std::println("total_distance: {}", res.total_distance);
    std::println("order: {}", res.order);
    print_seek_path(requests, res.order, ctx.min_cylinder, ctx.max_cylinder);
}

TEST_F(DiskArmSchedulingTest, Scan)
{
    auto res = scan(ctx, requests);
    std::println("Scan initial direction: up");
    std::println("total_distance: {}", res.total_distance);
    std::println("order: {}", res.order);
    print_seek_path(requests, res.order, ctx.min_cylinder, ctx.max_cylinder);
}

TEST_F(DiskArmSchedulingTest, BiScan)
{
    auto res = bi_scan(ctx, requests);
    std::println("Bidirectional Scan initial direction: up");
    std::println("total_distance: {}", res.total_distance);
    std::println("order: {}", res.order);
    print_seek_path(requests, res.order, ctx.min_cylinder, ctx.max_cylinder);
}

} // namespace shelpam::disk_arm_scheduling

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
