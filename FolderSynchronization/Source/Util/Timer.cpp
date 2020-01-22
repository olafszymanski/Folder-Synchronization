#include "pch.h"

#include "Timer.h"

Timer::Timer()
	: m_Start(std::chrono::high_resolution_clock::now())
	, m_Stopped(false)
{
}
Timer::~Timer()
{
	if (!m_Stopped) Stop();
}

long long Timer::Stop()
{
	long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_Start).count();

	m_Stopped = true;

	return duration;
}