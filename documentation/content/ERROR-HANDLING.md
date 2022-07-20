# Error handling

You can set callback for handling errors via `TccWrapper::SetErrorCallback`.

### With function

```cpp
void MyErrorHandler(void* userData, char const* msg)
{
    std::ostream& out = *reinterpret_cast<std::ostream*>(userData);

    out << msg << '\n';
}

int main()
{
    auto wrapper = tw::TccWrapper::WithState();

    wrapper.SetErrorCallback(&std::cerr, &MyErrorHandler);

    wrapper.AddFile("made/up/file.c");

    wrapper.Compile(); // tcc: error: file 'made/up/file.c' not found
}
```

### With stateless lambda

```cpp
int main()
{
    auto wrapper = tw::TccWrapper::WithState();

    wrapper.SetErrorCallback(&std::cerr, [](void* userData, char const* msg) {
        std::ostream& out = *reinterpret_cast<std::ostream*>(userData);

        out << msg << '\n';
    });

    wrapper.AddFile("made/up/file.c");

    wrapper.Compile(); // tcc: error: file 'made/up/file.c' not found
}
```
