#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <ranges>
#include <span>
#include <vector>

namespace shelpam::disk_arm_scheduling {

using Position = int;

struct Context {
    Position min_cylinder;
    Position max_cylinder;
    Position current_position;
};

struct Result {
    int total_distance;
    std::vector<std::size_t> order;
};

Result fcfs(Context ctx, std::span<Position const> requests)
{
    Result result{};
    result.order.resize(requests.size());
    // libc++ hasn't implemented the ranges version
    std::iota(result.order.begin(), result.order.end(), 0);
    for (auto req : requests) {
        result.total_distance += std::abs(ctx.current_position - req);
        ctx.current_position = req;
    }
    return result;
}

void move_to(Position dest, Context &ctx, int &distance)
{
    distance += std::abs(dest - ctx.current_position);
    ctx.current_position = dest;
}

/// @brief: Shortest Seek Time First
Result sstf(Context ctx, std::span<Position const> requests)
{
    Result result{};

    auto dist2cur = [&ctx, requests](int idx) {
        return std::abs(ctx.current_position - requests[idx]);
    };

    std::vector<int> used(requests.size(), 0);
    for (int _{}; _ != requests.size(); ++_) {
        // Every time select one of the shortest seek time.
        int min_dist_idx{}; // Index of minimum distance
        for (int j{}; j != requests.size(); ++j) {
            if (used[j] == 0 && dist2cur(min_dist_idx) > dist2cur(j)) {
                min_dist_idx = j;
            }
        }
        assert(used[min_dist_idx] == 0);
        result.total_distance += dist2cur(min_dist_idx);
        result.order.push_back(min_dist_idx);
        used[min_dist_idx] = 1;
        ctx.current_position = requests[min_dist_idx];
    }

    return result;
}

enum class Direction : std::int8_t { up, down };

/// @brief: Elevator algorithm
Result elevator(Context ctx, std::span<Position const> requests,
                Direction initial_direction)
{
    Result result{};

    std::vector<std::size_t> indices(requests.size());
    // libc++ hasn't implemented the ranges version
    std::iota(indices.begin(), indices.end(), 0);
    std::ranges::sort(indices, {}, [&requests](int i) { return requests[i]; });

    // From one end to the another end
    result.total_distance +=
        requests[indices.back()] - requests[indices.front()];
    switch (initial_direction) {
    case Direction::down:
        // From current_position to bottommost
        result.total_distance +=
            std::abs(ctx.current_position - requests[indices.front()]);
        result.order = indices;
        break;
    case Direction::up:
        // From current_position to topmost
        result.total_distance +=
            std::abs(ctx.current_position - requests[indices.back()]);
        result.order =
            indices | std::views::reverse | std::ranges::to<std::vector>();
        break;
    }
    return result;
}

Result scan(Context ctx, std::span<Position const> requests)
{
    Result result{};

    std::vector<std::size_t> indices(requests.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::ranges::sort(indices, {}, [&requests](int i) { return requests[i]; });

    auto delim =
        std::ranges::lower_bound(indices, ctx.current_position, {},
                                 [&requests](auto i) { return requests[i]; });

    // 1. Traverse to rightmost
    for (auto it = delim; it != indices.end(); ++it) {
        move_to(requests[*it], ctx, result.total_distance);
        result.order.push_back(*it);
    }
    move_to(ctx.max_cylinder, ctx, result.total_distance);

    // 2. Move to leftmost
    move_to(ctx.min_cylinder, ctx, result.total_distance);

    // 3. Move to scan left positions
    for (auto it = indices.begin(); it != delim; ++it) {
        move_to(requests[*it], ctx, result.total_distance);
        result.order.push_back(*it);
    }

    return result;
}

Result bi_scan(Context ctx, std::span<Position const> requests)
{
    Result result{};

    std::vector<std::size_t> indices(requests.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::ranges::sort(indices, {}, [&requests](int i) { return requests[i]; });

    auto delim =
        std::ranges::lower_bound(indices, ctx.current_position, {},
                                 [&requests](auto i) { return requests[i]; });

    // 1. Traverse to rightmost
    for (auto it = delim; it != indices.end(); ++it) {
        move_to(requests[*it], ctx, result.total_distance);
        result.order.push_back(*it);
    }
    move_to(ctx.max_cylinder, ctx, result.total_distance);

    // 2. Move to leftmost
    if (delim != indices.begin()) { // Any remaining
        auto it = delim;
        do {
            --it;
            move_to(requests[*it], ctx, result.total_distance);
            result.order.push_back(*it);
        } while (it != indices.begin());
    }

    return result;
}

} // namespace shelpam::disk_arm_scheduling
