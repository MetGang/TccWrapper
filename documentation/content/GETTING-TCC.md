# Getting tcc

### Building

To build underlyling tcc library follow these 3 simple steps:

* Clone code from [official repository](https://repo.or.cz/tinycc.git) or [unofficial GitHub mirror](https://github.com/TinyCC/tinycc).
* Run `configure` script (look below for available options).
* Run `make` command for newly created Makefile.

After that you should have `libtcc` and `libtcc1` library files (`.a`, `.so`, `.dll` or whatever depending on your OS and config).

`configure` script help message with all the options:

```
Usage: configure [options]
Options: [defaults in brackets after descriptions]

Standard options:
  --help                   print this message
  --prefix=PREFIX          install in PREFIX [C:/Program Files/tcc]
  --exec-prefix=EPREFIX    install architecture-dependent files in EPREFIX
			   [same as prefix]
  --bindir=DIR             user executables in DIR [EPREFIX/bin]
  --libdir=DIR             object code libraries in DIR [EPREFIX/lib]
  --tccdir=DIR             installation directory [EPREFIX/lib/tcc]
  --includedir=DIR         C header files in DIR [PREFIX/include]
  --sharedir=DIR           documentation root DIR [PREFIX/share]
  --docdir=DIR             documentation in DIR [SHAREDIR/doc/tcc]
  --mandir=DIR             man documentation in DIR [SHAREDIR/man]
  --infodir=DIR            info documentation in DIR [SHAREDIR/info]

Advanced options (experts only):
  --source-path=PATH       path of source code [.]
  --cross-prefix=PREFIX    use PREFIX for compile tools []
  --sysroot=PREFIX         prepend PREFIX to library/include paths []
  --cc=CC                  use C compiler CC [gcc]
  --ar=AR                  create archives using AR [ar]
  --extra-cflags=          specify compiler flags [-Wall -O2]
  --extra-ldflags=         specify linker options []
  --cpu=CPU                CPU [x86_64]

  --debug                  include debug info with resulting binaries
  --disable-static         make libtcc.so instead of libtcc.a
  --enable-static          make libtcc.a instead of libtcc.dll (win32)
  --disable-rpath          disable use of -rpath with libtcc.so
  --with-libgcc            use libgcc_s.so.1 instead of libtcc1.a
  --with-selinux           use mmap for executable memory (tcc -run)
  --enable-cross           build cross compilers (see also 'make help')

  --sysincludepaths=...    specify system include paths, colon separated
  --libpaths=...           specify system library paths, colon separated
  --crtprefix=...          specify locations of crt?.o, colon separated
  --elfinterp=...          specify elf interpreter
  --triplet=...            specify system library/include directory triplet

  --config-uClibc,-musl    enable system specific configurations
  --config-mingw32         build on windows using msys, busybox, etc.
  --config-backtrace=no    disable stack backtraces (with -run or -bt)
  --config-bcheck=no       disable bounds checker (-b)
  --config-predefs=no      do not compile tccdefs.h, instead just include
```

### Linking

After building tcc you are presented with `libtcc` and `libtcc1` files.

`libtcc` needs to be linked to your main program. On some non-win32 platforms linking additionally `dl` and/or `pthread` may be required.

`libtcc1` is linked internally by tcc while compiling scripts. You can add linking directories with `tcc_add_library_path` or `TccWrapper::AddLibraryPath`.
