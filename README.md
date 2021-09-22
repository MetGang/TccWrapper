# TccWrapper

Convenient header-only C++17 wrapper to use with embedded [Tiny C Compiler (tcc)](https://bellard.org/tcc/) from [official repository](https://repo.or.cz/tinycc.git).

## Usage

#### Basics

* script.c
```c
extern void HelloWorld();

int main()
{
    HelloWorld();

    return 0;
}
```

* main.cpp
```cpp
#include <iostream>

#include <TccWrapper.hpp>

void HelloWorld()
{
    std::cout << "Hello world!" << '\n';
}

int main()
{
    // Create wrapper object
    auto tcc = tw::TccWrapper::New();

    // Create context (instance of wrapped tcc)
    if (!tcc.CreateContext())
    {
        std::cerr << "Cannot create valid context" << '\n';

        return 1;
    }

    // Add file with C source code for compilation
    tcc.AddFile("script.c");

    // Register function for use within script
    tcc.RegisterFunction("HelloWorld", &HelloWorld);

    // Compile code
    tcc.Compile();

    // Call 'main' function from C script
    std::cout << tcc.Call<int>("main") << '\n';
}
```

## License

Just see [license](LICENSE).
