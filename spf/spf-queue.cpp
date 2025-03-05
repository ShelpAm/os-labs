#include <spf/spf-queue.hpp>

void SPF_queue::enqueue(Process_type job)
{
    auto it{list_.begin()};
    while (it != list_.end() && job->execution_time >= (*it)->execution_time) {
        ++it;
    }

    list_.insert(it, job);
}

SPF_queue::Process_type SPF_queue::dequeue()
{
    auto ret{list_.front()};
    list_.pop_front();
    return ret;
}

bool SPF_queue::empty() const
{
    return list_.empty();
}
