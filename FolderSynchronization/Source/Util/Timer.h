#pragma once

class Timer
{
public:
	Timer();
	~Timer();

	long long Stop();

private:
	std::chrono::time_point<std::chrono::steady_clock> m_Start;

	bool m_Stopped;
};