#pragma once

#include <list>
#include <memory>
#include <process/process.hpp>

class SPF_queue {
    using Process_type = std::shared_ptr<Process>;

  public:
    void enqueue(Process_type job);
    Process_type dequeue();
    [[nodiscard]] bool empty() const;

  private:
    std::list<Process_type> list_;
};
