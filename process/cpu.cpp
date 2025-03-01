#include <process/cpu.hpp>
#include <process/process.hpp>

void CPU::run_for(int minutes)
{
    if (running_process_ != nullptr) {
        running_process_->run_for(minutes, *this);
    }

    system_time_.minute += minutes;
}

void CPU::set_running(std::shared_ptr<Process> const &process)
{
    running_process_ = process;
}

Process const *CPU::running_process()
{
    return running_process_.get();
}
