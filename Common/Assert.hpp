#pragma once

#include "Logger.hpp"
#include "Macro.hpp"

// #include <filesystem>

#if defined(ZRN_ENABLE_ASSERTS)
    #define _ZRN_ASSERT_IMPL(type, condition, message, ...) \
    {                                                       \
        if (!(condition))                                   \
        {                                                   \
            ZRN##type##ERROR(message, __VA_ARGS__);         \
            ZRN_DEBUGBREAK();                               \
        }                                                   \
    }                                                       \

    #define _ZRN_ASSERT_MSG(type, condition, ...)           \
        _ZRN_ASSERT_IMPL(                                   \
            type,                                           \
            condition,                                      \
            "Assertion failed: {0}",                        \
            __VA_ARGS__                                     \
        )                                                   \
                                                          

    #define _ZRN_ASSERT_NO_MSG(type, condition)             \
        _ZRN_ASSERT_IMPL(                                   \
            type,                                           \
            condition,                                      \
            "Assertion '{0}' failed at {1} : {2}",          \
            ZRN_STRINGIFY_MACRO(condition),                 \
            __FILE__,                                       \
            __LINE__                                        \
        )                                                   \

    #define _ZRN_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro

    #define _ZRN_ASSERT_GET_MACRO(...)                      \
        ZRN_EXPAND_MACRO(                                   \
            _ZRN_ASSERT_GET_MACRO_NAME(                     \
                __VA_ARGS__,                                \
                _ZRN_ASSERT_MSG,                            \
                _ZRN_ASSERT_NO_MSG                          \
            )                                               \
        )                                                   \

    #define ZRN_ASSERT(...) \
                ZRN_EXPAND_MACRO(_ZRN_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))

    #define ZRN_CORE_ASSERT(...) \
                ZRN_EXPAND_MACRO(_ZRN_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))

#else
    #define ZRN_ASSERT(...)
    #define ZRN_CORE_ASSERT(...)
#endif