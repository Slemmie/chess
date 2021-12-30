#pragma once

#include <unordered_map>

namespace ai01 {
	
	class Move_table {
		
	public:
		
		inline uint8_t count(const uint32_t hash) {
			return m_umap[hash];
		}
		
		inline void push(const uint32_t hash) {
			m_umap[hash]++;
		}
		
		inline void pop(const uint32_t hash) {
			auto it = m_umap.find(hash);
			if (--it->second <= 0) {
				m_umap.erase(it);
			}
		}
		
	private:
		
		std::unordered_map <uint32_t, uint8_t> m_umap;
		
	};
	
} /// namespace ai01
