#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"



class Log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
private:
	static std::shared_ptr<spdlog::logger> s_Logger;

};


#define CORE_TRACE(...)         ::Log::GetLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)          ::Log::GetLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)          ::Log::GetLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)         ::Log::GetLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...)      ::Log::GetLogger()->critical(__VA_ARGS__)