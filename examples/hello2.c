extern void printf(char const*, ...);

char const* hello = "Hello, ";

#ifdef _WIN32
__attribute__((dllimport))
#endif
extern char const world[];

int main()
{
    printf(world);

    return 0;
}
