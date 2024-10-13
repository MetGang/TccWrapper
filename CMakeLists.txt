cmake_minimum_required( VERSION 3.12 )

project( TccWrapper
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "Convenient header-only C++17 wrapper to use with embedded Tiny C Compiler (tcc)."
    HOMEPAGE_URL "https://github.com/MetGang/TccWrapper"
)

target_compile_features( TccWrapper INTERFACE cxx_std_17 )

target_include_directories( TccWrapper INTERFACE "include/" )

add_library( TccWrapper INTERFACE )
