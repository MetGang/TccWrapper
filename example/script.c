#include <stdio.h>

typedef intptr_t Handle_t;

float ScriptFunc()
{
    return 1.5f;
}

export int scriptVar = 0;

import int universeNumber;
import float pi;

// import void PrintHello(); // C allows implicit declarations
import int GetFour();

import void Foo_Bar(Handle_t*, int);

int main()
{
    puts("This is " NAME " library with version " VER " (not real version btw)");

    PrintHello();

    printf("This is four -> %i\n", GetFour());
    printf("%i is the number of the universe but %.5f is even more important\n", universeNumber, pi);

    import Handle_t foo;
    Foo_Bar(&foo, 20);

    printf("scriptVar which got changed in C++ -> %i", scriptVar);

    return 0;
}
