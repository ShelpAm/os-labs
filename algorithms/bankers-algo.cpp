#include <algorithms/bankers-algo.hpp>
#include <cassert>
#include <print>
#include <ranges>
#include <set>
#include <unordered_set>

using namespace shelpam;

bankers_algo::Resources
bankers_algo::operator-(bankers_algo::Resources lhs,
                        bankers_algo::Resources const &rhs)
{
    return lhs -= rhs;
}

bankers_algo::Process::Process(std::string name, Resources maximum,
                               System *system)
    : name(std::move(name)), maximum(std::move(maximum)), system(system)
{
}

bankers_algo::Request_result
bankers_algo::Process::try_request(Resources const &req)
{
    assert(system != nullptr);
    std::println("Process {} trying requesting resources below:\n{}\n", name,
                 to_string(req));
    auto result = system->try_allocate(*this, req);

    if (all_greater_equal({}, need())) { // if no further need,
        finish();                        // return requested resources
    }

    return result;
}

bankers_algo::System::System(Resources const &total)
    : total_(total), available_(total)
{
}

bankers_algo::Request_result
bankers_algo::System::try_allocate(Process &p, Resources const &req)
{
    // Assume that req won't outsize p.need()
    if (!can_grant(req)) {
        std::println("Failed to allocate given request. system "
                     "available not enough.\n{}\n",
                     to_string(available_));
        return Request_result::no_enough_resources;
    }

    p.current_allocated += req;
    available_ -= req;

    auto safety_result = check_safe_state();
    if (!safety_result.is_safe) {
        // Restore original resources
        p.current_allocated -= req;
        available_ += req;
        std::println("Failed to allocate given request, no safe sequence.");
        return Request_result::no_safe_sequence;
    }

    std::print("Allocation successful, one of the safe sequence(s): {}",
               safety_result.safe_sequence.front());
    for (auto const &e : safety_result.safe_sequence | std::views::drop(1)) {
        std::print(" -> {}", e);
    }
    std::println("");
    std::println("System available resources:\n{}", to_string(available_));
    std::println("");
    return Request_result::success;
}

// TODO(shelpam): Please optimize this to O(N log N)
bankers_algo::Safe_state bankers_algo::System::check_safe_state() const
{
    std::vector<std::string> safe_seq;
    std::unordered_set<Process const *> safe_seq_set;

    auto can_use = available_;
    auto ok = [&can_use, &safe_seq_set](Process const &p) {
        return !safe_seq_set.contains(&p) && !p.finished() &&
               all_greater_equal(can_use, p.need());
    };

    auto num_processes = std::ranges::count_if(
        processes_, [](auto const &p) { return !p.finished(); });

    // Iterate |processes| times, to fech a safe seq of length |processes|.
    for (int i{}; i != num_processes; ++i) {
        auto it = std::ranges::find_if(processes_, ok);
        if (it == processes_.end()) {
            return {.is_safe = false, .safe_sequence = {}};
        }

        safe_seq.push_back(it->name);
        safe_seq_set.insert(&*it);
        can_use += it->current_allocated;
    }

    return {.is_safe = true, .safe_sequence = safe_seq};
}

bankers_algo::Process *
bankers_algo::System::make_process(std::string process_name, Resources maximum)
{
    processes_.push_back(
        Process{std::move(process_name), std::move(maximum), this});
    return &processes_.back();
}

auto &bankers_algo::Resources::operator[](Resource_kind const &resource)
{
    return count[resource];
}

bankers_algo::Resources &
bankers_algo::Resources::operator-=(Resources const &rhs)
{
    for (auto const &[k, v] : rhs.count) {
        count[k] -= v;
    }
    return *this;
}
bankers_algo::Resources &
bankers_algo::Resources::operator+=(Resources const &rhs)
{
    for (auto const &[k, v] : rhs.count) {
        count[k] += v;
    }
    return *this;
}
bankers_algo::Resources const &bankers_algo::System::available() const
{
    return available_;
}
void bankers_algo::Process::finish()
{
    if (finished_) {
        throw std::runtime_error("a process can't re-finish itself");
    }
    finished_ = true;
    assert(current_allocated.count == maximum.count);
    system->available_ += current_allocated;
    current_allocated.count.clear();

    std::println("All resources of process {} have been aquired, returning "
                 "them back to the OS.",
                 name);
    std::println("System available resources:\n{}",
                 to_string(system->available_));
    std::println("");
}
std::string bankers_algo::to_string(Resources const &r)
{
    std::string head;
    std::string body;
    for (auto [k, v] : r.count) {
        head += std::format("{:<5} ", k);
        body += std::format("{:<5} ", v);
    }
    return std::format("{}\n{}", head, body);
}
void bankers_algo::output_process_info(Process const &p)
{
    std::println("Process {}:", p.name);
    std::println("    maximum:\n{}:", to_string(p.maximum));
    std::println("    allocated:\n{}:", to_string(p.current_allocated));
    std::println("    need:\n{}:", to_string(p.need()));
    std::println();
}
bool bankers_algo::System::can_grant(Resources const &request) const
{
    return all_greater_equal(available_, request);
}
bool bankers_algo::all_greater_equal(Resources const &lhs, Resources const &rhs)
{
    std::set<Resource_kind> keys;
    keys.insert_range(lhs.count | std::views::keys);
    keys.insert_range(rhs.count | std::views::keys);

    return std::ranges::all_of(keys, [&lhs, &rhs](auto const &k) {
        return (lhs.count.contains(k) ? lhs.count.at(k) : 0) >=
               (rhs.count.contains(k) ? rhs.count.at(k) : 0);
    });
}
