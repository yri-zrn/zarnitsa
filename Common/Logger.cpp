#include "Logger.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

#include <assert.h>

namespace zrn
{

bool Log::Initialized = false;

std::shared_ptr<spdlog::logger> Log::CoreLogger;
std::shared_ptr<spdlog::logger> Log::ClientLogger;

void Log::Init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    
    CoreLogger = spdlog::stdout_color_mt("Zrn");
    assert(CoreLogger);
    CoreLogger->set_level(spdlog::level::trace);

    ClientLogger = spdlog::stdout_color_mt("App");
    assert(ClientLogger);
    ClientLogger->set_level(spdlog::level::trace);

    Initialized = true;
}

} // namespace zrn