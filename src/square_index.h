#pragma once

#include <string>

#define NO_SQUARE (1 << 6)

struct Square_index {
	
	Square_index() : m_rankfile(NO_SQUARE) { }
	Square_index(const uint8_t _rank, const uint8_t _file) : m_rankfile((_rank << 3) | _file) { }
	Square_index(const uint8_t _rankfile) : m_rankfile(_rankfile) { }
	Square_index(const std::string& _str) : m_rankfile(Square_index::string_to_rankfile(_str)) { }
	
	inline bool is_invalid() const {
		return m_rankfile & NO_SQUARE;
	}
	
	inline uint8_t rank() const {
		return (m_rankfile >> 3) & 0b111;
	}
	
	inline uint8_t file() const {
		return (m_rankfile >> 0) & 0b111;
	}
	
	inline uint8_t rankfile() const {
		return m_rankfile;
	}
	
	inline std::string to_string() const {
		std::string result;
		result += 'A' + ((m_rankfile >> 0) & 0b111);
		result += '8' - ((m_rankfile >> 3) & 0b111);
		return result;
	}
	
	static inline uint8_t string_to_rankfile(const std::string& str) {
		if (islower(str[0])) {
			return ((8 - (str[1] - '0')) << 3) | (str[0] - 'a');
		}
		return ((8 - (str[1] - '0')) << 3) | (str[0] - 'A');
	}
	
	inline bool operator == (const Square_index& oth) const {
		return m_rankfile == oth.rankfile();
	}
	
private:
	
	uint8_t m_rankfile;
	
};
