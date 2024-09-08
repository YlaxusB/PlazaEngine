#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Plaza {
	void Log::Init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		sCoreLogger = spdlog::stdout_color_mt("PLAZA");
		sCoreLogger->set_level(spdlog::level::trace);

		sClientLogger = spdlog::stdout_color_mt("APP");
		sClientLogger->set_level(spdlog::level::trace);
	}
}