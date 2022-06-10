# Wrapper creation

TccWrapper provides variety ways to be instantiated. Instead of traditional constructors you are presented with Rust-like object creation where static methods are called to return the wrapper object.

### Empty wrapper

You can create empty (invalid) wrapper to postpone state creation.

```cpp
auto wrapper = tw::TccWrapper::New();
```

Then you need to call `CreateState` to be able to operate on it.

```cpp
if (!wrapper.CreateState())
{
    std::cerr << "Couldn't create tcc state" << '\n';
}
```

### With state

State can be created within wrapper creation. Throws on failure.

```cpp
auto wrapper = tw::TccWrapper::WithState();
```

### As `std::optional` with state

State can be created within wrapper creation. Returns empty optional on failure.

```cpp
auto wrapper = tw::TccWrapper::OptWithState();

if (!wrapper)
{
    std::cerr << "Couldn't create tcc state" << '\n';
}
```

### From existing state

You can also create wrapper from existing TCCState state which will be further managed by the wrapper. Remember that TccWrapper won't check whether sent state is a valid one.

```cpp
TCCState* state = tcc_new();

auto wrapper = tw::TccWrapper::From(state);
```

### From `nullptr`

Won't compile.

```cpp
auto wrapper = tw::TccWrapper::From(nullptr); // compilation error
```
