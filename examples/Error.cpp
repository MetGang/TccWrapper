#include <TccWrapper.hpp>

#include <iostream>

auto main() -> int
{
    auto tcc = tw::TccWrapper{};

    tcc.create_state();

    tcc.set_error_callback(&std::cerr, +[](void* user_data, char const* msg) {
        std::ostream& o = *reinterpret_cast<std::ostream*>(user_data);
        o << "< " << msg << " >" << '\n';
    });

    tcc.add_file("error.c");

    tcc.compile();

    return 0;
}
