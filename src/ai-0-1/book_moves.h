#pragma once

#include <game.h>

#include <util/random.h>

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <cassert>
#include <vector>
#include <algorithm>

namespace ai01 {
	
	class Book_moves {
		
	public:
		
		Book_moves(const std::string& _filepath) {
			std::ifstream inf(_filepath);
			if (!inf.is_open()) {
				std::cerr << "failed to open file '" << _filepath << "'" << std::endl;
				std::cerr << "failed to read book" << std::endl;
				return;
			}
			std::map <std::pair <uint32_t, std::pair <std::string, uint8_t>>, int32_t> mp;
			std::string line;
			while (std::getline(inf, line)) {
				std::vector <std::string> moves;
				std::string buffer;
				for (char c : line) {
					if (c == ' ') {
						moves.emplace_back(buffer);
						buffer.clear();
						continue;
					}
					buffer += c;
				}
				int32_t game_result = 0;
				if (buffer == "1/2-1/2") {
					game_result = 0;
				} else if (buffer == "1-0") {
					game_result = 1;
				} else if (buffer == "0-1") {
					game_result = -1;
				} else {
					continue;
				}
				Board board(DEFAULT_START_POS, false);
				for (const std::string& move : moves) {
					uint32_t hash = Board_hash::hash(board);
					mp[{ hash, { move, board.turn() } }] += game_result;
					board = Board(board, Move(move));
				}
			}
			inf.close();
			std::map <uint32_t, std::vector <std::pair <int32_t, std::string>>> white, black;
			for (auto& p : mp) {
				if (p.first.second.second == COLOR_WHITE) {
					white[p.first.first].emplace_back(p.second, p.first.second.first);
				} else {
					black[p.first.first].emplace_back(p.second, p.first.second.first);
				}
			}
			for (auto& p : white) {
				std::sort(p.second.rbegin(), p.second.rend());
				for (const auto& candidate : p.second) {
					if (abs(candidate.first - p.second[0].first) <= abs(p.second[0].first / 2)) {
						m_map[p.first].emplace_back(candidate.second);
					}
				}
			}
			for (auto& p : black) {
				std::sort(p.second.begin(), p.second.end());
				for (const auto& candidate : p.second) {
					if (abs(candidate.first - p.second[0].first) <= abs(p.second[0].first / 2)) {
						m_map[p.first].emplace_back(candidate.second);
					}
				}
			}
		}
		
		inline bool count(const uint32_t hash) const {
			return m_map.find(hash) != m_map.end();
		}
		
		inline Move find(const uint32_t hash) const {
			auto it = m_map.find(hash);
			assert(it != m_map.end());
			return it->second[Random::rs32(0, (int32_t)it->second.size() - 1)];
		}
		
	private:
		
		std::map <uint32_t, std::vector <Move>> m_map;
		
	};
	
} /// namespace ai01
