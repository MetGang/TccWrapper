# TccWrapper

Convenient header-only C++17 wrapper to use with embedded [Tiny C Compiler (tcc)](https://bellard.org/tcc/).

## Usage

#### Building library

Since TccWrapper is a header-only library you only need to build wrapped **tcc** library. To do so:

* Clone code from [official repository](https://repo.or.cz/tinycc.git) or [unofficial GitHub mirror](https://github.com/TinyCC/tinycc).
* Run `configure` script from main directory.
* Run `make` command for newly created `Makefile`.

Now you should have `libtcc` and `libtcc1` (`.a`, `.so`, `.dll` or whatever depending on OS) library files in the main directory.

`configure` script has many options to suit your needs with most useful:

* `--cc=CC` - use C compiler CC
* `--ar=AR` - create archives using AR
* `--extra-cflags="FLAGS"` - specify additional compiler flags
* `--extra-ldflags="FLAGS"` - specify additional linker options
* `--debug` - include debug info with resulting binaries
* `--disable-static` - make libtcc.so instead of libtcc.a
* `--enable-static` - make libtcc.a instead of libtcc.dll (win32)

#### Compiling and running your program

Everything you need is to link `libtcc` (on non-win32 platforms you may need `dl` and `pthread` too) to your program and put `libtcc1` in the directory of executable (you can add custom directories with `TccWrapper::AddLibraryPath` method).

## Wiki

Please check repo's wiki for any kind of tutorials or examples.

## License

MIT, see [license](LICENSE) for more information.
