#pragma once

#include <cstddef>
#include <string_view>

namespace shelpam::memory {

class Allocator {
  public:
    Allocator() = default;
    Allocator(Allocator const &) = default;
    Allocator(Allocator &&) = delete;
    Allocator &operator=(Allocator const &) = default;
    Allocator &operator=(Allocator &&) = delete;
    virtual ~Allocator() = default;

    // Return:
    //  nullptr  if no available space
    virtual void *allocate(std::size_t size) = 0;

    virtual void deallocate(void *ptr) = 0;
};

} // namespace shelpam::memory
