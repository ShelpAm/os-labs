#include <spf/spf-queue.hpp>

void SPF_queue::enqueue(Process *job)
{
    auto it{list_.begin()};
    while (it != list_.end() &&
           job->total_execution_time >= (*it)->total_execution_time) {
        ++it;
    }

    list_.insert(it, job);
}

Process *SPF_queue::dequeue()
{
    Process *ret{list_.front()};
    list_.pop_front();
    return ret;
}

bool SPF_queue::empty() const
{
    return list_.empty();
}

Process *SPF_queue::front()
{
    return list_.front();
}
