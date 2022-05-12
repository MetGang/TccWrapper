cmake_minimum_required(VERSION 3.12)

project(TccWrapper
    VERSION 2.0.0
    LANGUAGES CXX
    DESCRIPTION "Convenient header-only C++17 wrapper to use with embedded Tiny C Compiler (tcc)."
    HOMEPAGE_URL "https://github.com/MetGang/TccWrapper"
)

add_library(TccWrapper INTERFACE)

target_include_directories(TccWrapper INTERFACE include/)

target_compile_features(TccWrapper INTERFACE cxx_std_17)
