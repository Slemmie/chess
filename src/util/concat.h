#pragma once

#include <string>
#include <sstream>

static inline std::string concat() {
	return "";
}

template <typename A, typename... B> static inline std::string concat(const A& a, const B&... b) {
	std::stringstream stream;
	stream << a << concat(b...);
	return stream.str();
}
