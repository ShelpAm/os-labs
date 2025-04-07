#include <process/cpu.hpp>
#include <process/process.hpp>

using namespace shelpam;

void scheduling::CPU::run_for(int minutes)
{
    if (running_process_ != nullptr) {
        auto const actual_runtime{running_process_->run_for(minutes, *this)};
        slice_execution_time_ += actual_runtime;
    }
    else {
        // std::println("{} CPU idle", to_string(system_time_));
        // fast_io::debug_println("CPU is running nothing for ", minutes,
        //                        " minutes");
    }
    system_time_.minutes_ += minutes;
}

void scheduling::CPU::set_running(Process *process) noexcept
{
    running_process_ = process;
    slice_execution_time_ = 0; // Reset for new process
}

scheduling::Process *scheduling::CPU::running_process() const noexcept
{
    return running_process_;
}

int scheduling::CPU::slice_execution_time() const noexcept
{
    return slice_execution_time_;
}

scheduling::Time scheduling::CPU::system_time() const noexcept
{
    return system_time_;
}

void scheduling::CPU::skip_to(Time t)
{
    if (t < system_time_) {
        throw std::runtime_error(
            std::format("can't skip to past ({})", to_string(t)));
    }
    system_time_ = t;
}
