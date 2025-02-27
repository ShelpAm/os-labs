#include <fast-io-ext/fixed.hpp>
#include <fast_io.h>

int main()
{
    constexpr double d{1.23};
    fast_io::println(d);
    fast_io::println(fast_io::mnp::middle(d, 10));
    // fast_io::println(fast_io::mnp::fixed(d, 1));

    constexpr __uint128_t s{fast_io::to<__uint128_t>(
        "   2424", fast_io::mnp::left(0, 30, '0'),
        " sgdjasdopgjdasgpojadsgpoadjsgpoadsjgopjaghdsoighasdioghadsoi")};
    fast_io::println(s);

    auto const fixed{fast_io_ext::mnp::fixed(d, 1)};

    // auto width{fast_io::mnp::width(d, 1)};
    // fast_io ::println(fixed);
}
