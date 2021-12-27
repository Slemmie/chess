#pragma once

#include <board.h>

class Board_hash {
	
public:
	
	static uint32_t hash(const Board& board) {
		uint32_t result;
		uint64_t pref_pow = 1;
		for (uint8_t rank = 0; rank < 8; rank++) {
			for (uint8_t file = 0; file < 8; file++) {
				result = (pref_pow * board[rank][file].piece() + result) % m_mod;
				pref_pow = (pref_pow * m_base) % m_mod;
			}
		}
		return result;
	}
	
private:
	
	static inline uint64_t m_mod = 1'000'000'207;
	static inline uint64_t m_base = (10 << 2) | 5;
	
};
