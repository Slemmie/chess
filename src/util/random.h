#pragma once

#include <random>

class Random {
	
public:
	
	static double negpos1() {
		static std::random_device device;
		static std::mt19937 generator(device());
		static std::uniform_real_distribution <double> distribution(-1.0, 1.0);
		return distribution(generator);
	}
	
	static int64_t rs32(int32_t left, int32_t right) {
		left = std::min(left, right);
		static std::random_device device;
		static std::mt19937 generator(device());
		static std::uniform_int_distribution <int64_t> distribution(0.0, 1LL << 60);
		int64_t num = distribution(generator) % (right - left + 1);
		return num + left;
	}
	
};
