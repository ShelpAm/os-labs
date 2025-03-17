#include <spf/spf-queue.hpp>

void SPF_queue::push(Process *job)
{
    auto it{list_.begin()};
    while (it != list_.end() &&
           job->total_execution_time >= (*it)->total_execution_time) {
        ++it;
    }

    list_.insert(it, job);
}

void SPF_queue::pop()
{
    list_.pop_front();
}

bool SPF_queue::empty() const
{
    return list_.empty();
}

Process *SPF_queue::front()
{
    return list_.front();
}

std::vector<Process::Id> to_vector(SPF_queue q)
{
    std::vector<Process::Id> result;

    while (!q.empty()) {
        result.push_back(q.front()->id);
        q.pop();
    }

    return result;
}
