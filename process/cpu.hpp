#pragma once

#include <process/time.hpp>

#include <memory>

struct Process;

class CPU {
    using Process_type = std::shared_ptr<Process>;

  public:
    void run_for(int minutes);

    [[nodiscard]] Time now() const
    {
        return system_time_;
    }

    void set_running(Process_type const &process);

    Process const *running_process();

  private:
    Time system_time_{};
    Process_type running_process_;
};
