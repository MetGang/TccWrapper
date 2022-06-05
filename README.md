# TccWrapper

Convenient header-only C++17 wrapper to use with embedded [Tiny C Compiler (tcc)](https://bellard.org/tcc/).

## Usage

Please refer to repository wiki or comments in [TccWrapper.hpp](include/TccWrapper.hpp) for any kind of help.

#### Very quick start

```c
// script.c
int main(void)
{
    return 0;
}
```

```cpp
// main.cpp
#define TW_USE_EXCEPTIONS
#define TW_USE_OPTIONAL
#include <TccWrapper.hpp>

int main()
{
    auto tcc = tw::TccWrapper::WithState();

    tcc.AddLibraryPath("/path/to/libtcc1");

    tcc.AddFile("script.c");

    tcc.Compile();

    return tcc.Invoke<int()>("main");
}

```

## Availability

TccWrapper requires at least C++17 capable compiler to work. Check [live preview](https://godbolt.org/z/d95vbe9Tn) for list of available compilers and their minimal supported versions. List may not be 100% accurate!

## License

MIT, see [license](LICENSE) for more information.
