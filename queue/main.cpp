#include <cassert>
#include <cstdlib>
#include <fast_io.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include <functional>
#include <stdexcept>
#include <utility>

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

void print_prompts()
{
    fast_io::io::println("1. new");
    fast_io::io::println("2. init");
    fast_io::io::println("3. pop");
    fast_io::io::println("4. push [value]");
    fast_io::io::println("5. print-all");
    fast_io::io::println("6. clear");
    fast_io::io::println("7. is-full");
    fast_io::io::println("8. is-empty");
    fast_io::io::println("9. exit");
}

void parse(Queue<int> &queue, bool &created, bool &should_exit,
           fast_io::string const &cmd)
{
    // queue-related operation but not creating
    if (cmd != "1" && cmd != "9" && !created) {
        fast_io::io::println("error: queue not created");
        return;
    }

    if (cmd == "1") {
        // Left blank, because queue has already been created before,
        // with little effort.
        // Directly simulate this.
        created = true;
    }
    else if (cmd == "2") {
        fast_io::io::println("Input the number of element:");
        int num;
        fast_io::io::scan(num);
        for (int i{}; i != num; ++i) {
            fast_io::io::print("The ", i + 1, "-th element: ");
            int value;
            fast_io::io::scan(value);
            queue.push(value);
        }
    }
    else if (cmd == "3") {
        if (queue.empty()) {
            fast_io::println("Cannot pop, queue is empty.");
            return;
        }
        fast_io::println("Poped element: ", queue.front());
        queue.pop();
    }
    else if (cmd == "4") {
        if (bool const full{queue.size() == Queue<int>::capacity}; full) {
            fast_io::println("Cannot push, queue is full.");
            return;
        }
        fast_io::io::print("To be pushed: ");
        int value;
        fast_io::io::scan(value);
        queue.push(value);
    }
    else if (cmd == "5") {
        fast_io::io::println("Elements in queue (in pushing order):");
        queue.for_each([](auto e) { fast_io::io::print(e, " "); });
        fast_io::io::println("");
    }
    else if (cmd == "6") {
        queue.clear();
    }
    else if (cmd == "7") {
        bool const full{queue.size() == Queue<int>::capacity};
        fast_io::io::println("The queue is ",
                             fast_io::mnp::os_c_str(full ? "" : "not "),
                             "full.");
    }
    else if (cmd == "8") {
        bool const empty{queue.empty()};
        fast_io::io::println("The queue is ",
                             fast_io::mnp::os_c_str(empty ? "" : "not "),
                             "empty.");
    }
    else if (cmd == "9") {
        should_exit = true;
    }
    else {
        fast_io::io::println("Invalid command, try again.");
    }
}

int main()
{
    print_prompts();

    Queue<int> queue;
    bool created{};
    bool should_exit{};

    while (!should_exit) {
        fast_io::io::print("Select an option: ");
        fast_io::string cmd;
        fast_io::io::scan(cmd);
        parse(queue, created, should_exit, cmd);
    };
    return EXIT_SUCCESS;
}
