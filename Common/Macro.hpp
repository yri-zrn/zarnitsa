#pragma once

#if defined(ZRN_WIN32)
    #define ZRN_DEBUGBREAK() __debugbreak()
#else
    #error "Unsupported platform"
#endif

#if defined(ZRN_DEBUG)
    #define ZRN_ENABLE_ASSERTS
#endif

#define ZRN_EXPAND_MACRO(x) x

#define _ZRN_STRINGIFY_MACRO(x) #x
#define ZRN_STRINGIFY_MACRO(x) _ZRN_STRINGIFY_MACRO(x)

#define _ZRN_GLUE_MACRO(x, y) x ## y
#define ZRN_GLUE_MACRO(x, y) _ZRN_GLUE_MACRO(x, y)

#if defined(ZRN_DEBUG)
    #define ZRN_ON_DEBUG(x) x
    #define ZRN_UNUSED(var) ((void)var);
    #define ZRN_NOT_IMPLEMENTED() ZRN_CORE_ASSERT(false, "Not implemented")
#else
    #define ZRN_ON_DEBUG(x)
    #define ZRN_UNUSED(var)
    #define ZRN_NOT_IMPLEMENTED()
#endif

// Taken from boost/current_function.hpp
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
    #define ZRN_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
    #define ZRN_FUNCTION_NAME __FUNCSIG__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
    #define ZRN_FUNCTION_NAME __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
    #define ZRN_FUNCTION_NAME __func__
#else
    # define ZRN_FUNCTION_NAME "(unknown)"
#endif