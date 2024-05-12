#define TW_USE_EXCEPTIONS
#define TW_USE_OPTIONAL
#include <TccWrapper.hpp>

auto main() -> int
{
    auto tcc = tw::TccWrapper::WithState();

    tcc.AddLibraryPath("lib");

    tcc.AddFile("basic.c");

    tcc.Compile();

    return tcc.Invoke<int()>("main");
}
