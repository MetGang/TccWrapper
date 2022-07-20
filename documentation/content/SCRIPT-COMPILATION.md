# Script compilation

TccWrapper provides 2 basic ways to compile scripts.

### With auto managed memory

Simply invoke `TccWrapper::Compile` to let tcc take care of all the work. By default tcc will use `malloc` to allocate code buffer.

```cpp
wrapper.Compile();
```

### With user managed buffer

Take care of code buffer by yourself.

```cpp
auto size = wrapper.GetCodeSize();

auto buffer = std::make_unique<std::byte[]>(size);

wrapper.Compile(buffer.get());
```
