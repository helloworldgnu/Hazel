#include "Log.h"

namespace Hazel{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        spdlog::set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] [%n] [%P:%t] [%s:%!():%#] %@ %v)");
        Log::s_CoreLogger = spdlog::stdout_color_mt("HAZEL", spdlog::color_mode::always);
        Log::s_CoreLogger->set_level(spdlog::level::trace);

        Log::s_ClientLogger = spdlog::stdout_color_mt("APP", spdlog::color_mode::always);
        Log::s_ClientLogger->set_level(spdlog::level::trace);
    }
}
