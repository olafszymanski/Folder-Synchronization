#pragma once

#include <spdlog/logger.h>

class Logger
{
public:
	static Logger& Get();
	~Logger() = default;

	// Getter
	inline std::shared_ptr<spdlog::logger>& GetLogger() const { return s_Logger; }

private:
	Logger() = default;

private:
	static Logger* s_Instance;

	static std::shared_ptr<spdlog::logger> s_Logger;
};

#ifdef _DEBUG
	#define LOG_LEVEL spdlog::level::trace

	#define LOG_DEBUG(...) Logger::Get().GetLogger()->debug(__VA_ARGS__)
	#define LOG_INFO(...) Logger::Get().GetLogger()->info(__VA_ARGS__)
	#define LOG_WARNING(...) Logger::Get().GetLogger()->warn(__VA_ARGS__)
	#define LOG_ERROR(...) Logger::Get().GetLogger()->err(__VA_ARGS__)
#else
	#define LOG_LEVEL spdlog::level::off

	#define LOG_DEBUG(...)
	#define LOG_INFO(...)
	#define LOG_WARNING(...)
	#define LOG_ERROR(...)
#endif