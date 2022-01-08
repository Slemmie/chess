#pragma once

#include <ext/pb_ds/assoc_container.hpp>

#include <cassert>

namespace ai01 {
	
	class Dp_map {
		
	public:
		
		inline void clear() {
			m_map.clear();
		}
		
		inline void insert(const double value, const uint32_t hash, const uint32_t depth) {
			auto it = m_map.find(hash);
			if (it == m_map.end() || depth > it->second.depth) {
				m_map[hash] = Dp_val { depth, value };
			}
		}
		
		inline bool count(const uint32_t hash, const uint32_t depth) const {
			auto it = m_map.find(hash);
			if (it == m_map.end() || depth > it->second.depth) {
				return false;
			}
			return true;
		}
		
		inline double find(const uint32_t hash, const uint32_t depth) const {
			auto it = m_map.find(hash);
			assert(it != m_map.end() && depth <= it->second.depth);
			return it->second.value;
		}
		
	private:
		
		struct Dp_val {
			
			uint32_t depth = 0;
			double value = 0;
			
		};
		
		__gnu_pbds::gp_hash_table <int32_t, Dp_val> m_map;
		
	};
	
} /// namespace ai01
