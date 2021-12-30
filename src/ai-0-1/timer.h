#pragma once

#include <chrono>

class Timer {
	
public:
	
	Timer() :
	m_begin_time(since_epoch())
	{ }
	
	inline int64_t current() {
		return since_epoch() - m_begin_time;
	}
	
	inline int64_t since_epoch() {
		return std::chrono::time_point_cast <std::chrono::microseconds>
		(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
	}
	
private:
	
	int64_t m_begin_time;
	
};
