#pragma once

#include <cassert>
#include <stdexcept>

template <typename T> class Queue {
  public:
    Queue() : data_{new T[capacity]} {}

    void clear() noexcept
    {
        size_ = 0;
        front_ = back_ = 0;
    }

    void pop()
    {
        if (data_ == nullptr) {
            throw std::runtime_error("queue hasn't been initialized");
        }

        if (size_ == 0) {
            throw std::runtime_error("queue is empty, nothing to pop");
        }

        ++front_;
        if (front_ == capacity) {
            front_ = 0;
        }
        --size_;
    }

    void push(T t)
    {
        if (data_ == nullptr) {
            throw std::runtime_error("queue hasn't been initialized");
        }

        if (size_ == capacity) {
            throw std::runtime_error("queue is full, cannot push to it");
        }

        data_[back_] = std::move(t);
        ++back_;
        if (back_ == capacity) {
            back_ = 0;
        }
        ++size_;
    }

    void for_each(std::invocable<T> auto &&f)
    {
        for (auto i{front_}; i != back_; ++i) {
            if (i >= size_) {
                i = 0;
            }
            f(data_[i]);
        }
    }

    T &front()
    {
        assert(data_ != nullptr && size_ != 0);
        return data_[front_];
    }

    [[nodiscard]] std::size_t size() const
    {
        return size_;
    }

    [[nodiscard]] bool empty() const
    {
        return size_ == 0;
    }

    static constexpr std::size_t capacity{64};

  private:
    T *data_{};
    std::size_t size_{};
    std::ptrdiff_t front_{};
    std::ptrdiff_t back_{};
};
