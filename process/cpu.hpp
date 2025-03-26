#pragma once

#include <process/time.hpp>

namespace shelpam::scheduling {

struct Process;

class CPU {
  public:
    void run_for(int minutes);

    [[nodiscard]] Time system_time() const noexcept;
    [[nodiscard]] int slice_execution_time() const noexcept;
    [[nodiscard]] Process *running_process() const noexcept;

    void set_running(Process *process) noexcept;

  private:
    Time system_time_;
    Process *running_process_{};
    int slice_execution_time_{};
};

} // namespace shelpam::scheduling
