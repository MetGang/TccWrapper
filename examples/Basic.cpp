#include <TccWrapper.hpp>

auto main() -> int
{
    auto tcc = tw::TccWrapper{};

    tcc.create_state();

    tcc.add_library_path("lib");

    tcc.add_file("basic.c");

    tcc.compile();

    return tcc.invoke<int()>("main");
}
