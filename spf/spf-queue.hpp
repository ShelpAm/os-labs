#pragma once

#include <deque>
#include <fast_io_dsal/list.h>
#include <list>
#include <memory>
#include <process/process.hpp>

class SPF_queue {
  public:
    void enqueue(Process *job);
    Process *dequeue();
    Process *front();
    [[nodiscard]] bool empty() const;

  private:
    fast_io::list<Process *> list_;
};
