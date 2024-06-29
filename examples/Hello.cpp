#include <TccWrapper.hpp>

auto main() -> int
{
    auto tcc = tw::TccWrapper{};

    tcc.create_state();

    tcc.add_file("hello.c");

    tcc.compile();

    return tcc.invoke<int()>("main");
}
