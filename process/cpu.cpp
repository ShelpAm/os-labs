#include <process/cpu.hpp>
#include <process/process.hpp>

void CPU::run_for(int minutes)
{
    if (running_process_ != nullptr) {
        auto const actual_runtime{running_process_->run_for(minutes, *this)};
        slice_execution_time_ += actual_runtime;
    }
    else {
        // fast_io::debug_println("CPU is running nothing for ", minutes,
        //                        " minutes");
    }
    system_time_.minutes_ += minutes;
}

void CPU::set_running(Process *process) noexcept
{
    running_process_ = process;
    slice_execution_time_ = 0; // Reset for new process
}

Process *CPU::running_process() const noexcept
{
    return running_process_;
}

int CPU::slice_execution_time() const noexcept
{
    return slice_execution_time_;
}

Time CPU::system_time() const noexcept
{
    return system_time_;
}
