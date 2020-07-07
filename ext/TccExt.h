/*

    Extension header for TccWrapper to ease scripting

    Created by Patrick Stritch

*/

#ifndef TWS_TccExt
#define TWS_TccExt

#ifdef _WIN32
    #define export __declspec(dllexport)
    #define import extern __declspec(dllimport)
#else
    #define export __attribute__((visibility("default")))
    #define import extern
#endif

typedef __SIZE_TYPE__    size_t;
typedef __PTRDIFF_TYPE__ ssize_t;
typedef __WCHAR_TYPE__   wchar_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __PTRDIFF_TYPE__ intptr_t;
typedef __SIZE_TYPE__    uintptr_t;

typedef signed char          int8_t;
typedef signed short int     int16_t;
typedef signed int           int32_t;
typedef signed long long int int64_t;
typedef unsigned char        uint8_t;
typedef unsigned short int   uint16_t;
typedef unsigned int         uint32_t;
typedef unsigned long int    uint64_t;

typedef void* handle_t;

#define NULL  ((void*)0)
#define true  1
#define false 0

#endif // TWS_TccExt
