#define TW_USE_OPTIONAL
#define TW_USE_EXCEPTIONS
#include <TccWrapper.hpp>

#include <iostream>

void PrintHello()
{
    std::cout << "Hello world!" << '\n';
}

void PrintFloat(float* f)
{
    std::cout << "Some float -> " << *f << '\n';
}

int GetFour()
{
    return 4;
}

class Foo
{
public:

    void Bar(int otherNumber)
    {
        std::cout << "Bar from Foo says " << m_number << " and " << otherNumber << '\n';
    }

private:

    int m_number = 10;
};

int main()
{
    tw::TccWrapper tcc;

    if (!tcc.CreateContext())
    {
        std::cerr << "Unable to create tcc context" << '\n';

        return -1;
    }

    int errorCode = 123;

    tcc.SetExtErrorCallback(&errorCode, +[](int* errorCode, const char* message) {
        std::cerr << *errorCode << " | " << message << '\n';
    });

    tcc.AddIncludePath("./win32/include");
    tcc.AddIncludePath("../ext");
    tcc.AddLibraryPath("./win32"); // just for Windows, Linux should search for libtcc1.a in "/usr/local/lib/tcc"
    tcc.SetOutputType(tw::OutputType::Memory);
    tcc.SetOptions("-O2 -Wall -std=c99");

    tcc.Define("NAME", "\"TccWrapper\"");
    tcc.Define("VER", R"("1.0.2")");

    int universeNumber = 42;
    tcc.AddSymbol("universeNumber", &universeNumber);

    float pi = 3.14159f;
    tcc.AddSymbol("pi", &pi);

    float tau = pi * 2.0f;
    tcc.AddSymbol("tau", &tau);

    tcc.RegisterFunction<&PrintHello>("PrintHello");
    tcc.RegisterFunction<&PrintFloat>("PrintFloat");
    tcc.RegisterFunction<&GetFour>("GetFour");

    Foo foo;
    tcc.AddSymbol("foo", &foo);
    tcc.RegisterMethod<&Foo::Bar>("Foo_Bar");

    tcc.AddFile("script.c");

    tcc.Compile();

    *tcc.GetSymbolAs<int>("scriptVar") = 23;

    auto scriptFunc = tcc.GetFunction<float(*)()>("ScriptFunc");
    std::cout << "Value from ScriptFunc called in C++ -> " << scriptFunc() << '\n';

    auto printNums = tcc.GetFunction<void(*)(size_t, ...)>("PrintNums");
    std::cout << "PrintNums -> "; printNums(3, 1, 2, 3); std::cout << '\n';

    return tcc.Call<int>("main");
}
