#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace shelpam::bankers_algo {

using Resource_kind = std::string;

struct Resources {
    std::map<Resource_kind, std::int32_t>
        count; // TODO(shelpam): REPLACE THIS WITH unordered_map, since it's
               // faster. We use map here to beautify the output
    // std::unordered_map<Resource_kind, std::int32_t> count;

    auto &operator[](Resource_kind const &resource);

    Resources &operator+=(Resources const &rhs);
    Resources &operator-=(Resources const &rhs);
    friend Resources operator-(Resources lhs, Resources const &rhs);
};
std::string to_string(Resources const &r);
bool all_greater_equal(Resources const &lhs, Resources const &rhs);

struct System;
class Process {
    friend class System;
    friend void output_process_info(Process const &p);

  public:
    using Id = std::uint32_t;
    bool try_request(Resources const &req);

  private:
    std::string name;
    Resources current_allocated;
    Resources maximum;
    System *system;
    bool finished_{};

    Process(std::string name, Resources maximum, System *system);

    [[nodiscard]] Resources need() const
    {
        return maximum - current_allocated;
    }

    [[nodiscard]] bool finished() const
    {
        return finished_;
    }

    // This will return all aquired resources to the OS (operating system).
    void finish();
};
void output_process_info(Process const &p);

struct Check_safe_state_result {
    bool is_safe;
    std::vector<std::string> safe_sequence;
};

class System {
    friend class Process;

  public:
    System(Resources const &total);
    Process *make_process(std::string process_name, Resources maximum);
    [[nodiscard]] Resources const &available() const;

  private:
    Resources total_;
    Resources available_;
    std::list<Process> processes_;

    [[nodiscard]] bool can_grant(Resources const &request) const;
    bool try_allocate(Process &p, Resources const &req);
    [[nodiscard]] Check_safe_state_result check_safe_state() const;
};

} // namespace shelpam::bankers_algo
