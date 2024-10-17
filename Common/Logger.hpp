#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <memory>

namespace zrn {

class Log {
public:
    static void Init();

    static bool Initialized;

    static std::shared_ptr<spdlog::logger> CoreLogger;
    static std::shared_ptr<spdlog::logger> ClientLogger;
};

} // namespace zrn

#define ZRN_CORE_TRACE(...)   ::zrn::Log::CoreLogger->trace(__VA_ARGS__)
#define ZRN_CORE_INFO(...)    ::zrn::Log::CoreLogger->info(__VA_ARGS__)
#define ZRN_CORE_WARN(...)    ::zrn::Log::CoreLogger->warn(__VA_ARGS__)
#define ZRN_CORE_ERROR(...)   ::zrn::Log::CoreLogger->error(__VA_ARGS__)
#define ZRN_CORE_FATAL(...)   ::zrn::Log::CoreLogger->critical(__VA_ARGS__)

#define ZRN_TRACE(...)        ::zrn::Log::ClientLogger->trace(__VA_ARGS__)
#define ZRN_INFO(...)         ::zrn::Log::ClientLogger->info(__VA_ARGS__)
#define ZRN_WARN(...)         ::zrn::Log::ClientLogger->warn(__VA_ARGS__)
#define ZRN_ERROR(...)        ::zrn::Log::ClientLogger->error(__VA_ARGS__)
#define ZRN_FATAL(...)        ::zrn::Log::ClientLogger->critical(__VA_ARGS__)