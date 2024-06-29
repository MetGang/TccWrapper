#include <TccWrapper.hpp>

#include <cassert>

auto main() -> int
{
    auto tcc = tw::TccWrapper{};

    tcc.create_state();

    tcc.add_file("fibonacci.c");

    tcc.compile();

    auto fibonacci = tcc.get_function<int(int)>("fibonacci");

    assert(fibonacci(0) == 0);
    assert(fibonacci(1) == 1);
    assert(fibonacci(2) == 1);
    assert(fibonacci(3) == 2);
    assert(fibonacci(4) == 3);
    assert(fibonacci(5) == 5);
    assert(fibonacci(6) == 8);
    assert(fibonacci(7) == 13);
    assert(fibonacci(8) == 21);
    assert(fibonacci(9) == 34);

    return 0;
}
