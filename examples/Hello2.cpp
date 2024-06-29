#include <TccWrapper.hpp>

#include <iostream>

auto main() -> int
{
    auto tcc = tw::TccWrapper{};

    tcc.create_state();

    char const* world = "World!";
    tcc.add_symbol("world", world);

    tcc.add_file("hello2.c");

    tcc.compile();

    auto hello = tcc.get_symbol_as<char const*>("hello");
    std::cout << *hello;

    return tcc.invoke<int()>("main");
}
