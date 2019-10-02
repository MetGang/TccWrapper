#include <TccExt.h>

#include <stdio.h>
#include <stdarg.h>

float ScriptFunc()
{
    return 1.5f;
}

void PrintNums(size_t n, ...)
{
    va_list vl;
    va_start(vl, n);
    for (size_t v = 0; v < n; ++v) printf("%i ", va_arg(vl, int));
    va_end(vl);
}

export int scriptVar = 0;

import int universeNumber;
import float pi;

import void PrintHello();
// import void PrintFloat(Handle_t f);
import int GetFour();

// import void Foo_Bar(Handle_t, int);

int main()
{
    puts("This is " NAME " library with version " VER " (not real version btw)");

    PrintHello();

    printf("This is four -> %i\n", GetFour());
    printf("%i is the number of the universe but %.5f is even more important\n", universeNumber, pi);

    import Handle_t foo;
    Foo_Bar(&foo, 20);

    import Handle_t tau;
    PrintFloat(&tau);

    printf("scriptVar which got changed in C++ -> %i\n", scriptVar);

    return 0;
}
