#include "pch.h"

#include "Logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

Logger* Logger::s_Instance = nullptr;

std::shared_ptr<spdlog::logger> Logger::s_Logger = nullptr;

Logger& Logger::Get()
{
	if (!s_Instance)
	{
		s_Instance = new Logger;
		
		s_Logger = spdlog::stdout_color_mt("Logger");
		s_Logger->set_level(LOG_LEVEL);
		s_Logger->set_pattern("%^[%L] [%I:%M:%S:%e %p]: %v%$");
	}

	return *s_Instance;
}