#pragma once

#include <deque>
#include <list>
#include <memory>
#include <process/process.hpp>
#include <vector>

class SPF_queue {
  public:
    void push(Process *job);
    void pop();
    Process *front();
    [[nodiscard]] bool empty() const;

  private:
    std::list<Process *> list_;
};

std::vector<Process::Id> to_vector(SPF_queue q);
