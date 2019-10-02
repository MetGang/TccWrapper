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

typedef void* Handle_t;

#endif
