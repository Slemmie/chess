// 64-bit bit board

#include <stdint.h>

namespace game {
	
	typedef uint64_t bit_board;
	
	namespace bb {
		
		constexpr bit_board all_squares =
		~static_cast <bit_board> (0);
		constexpr bit_board dark_squares =
		static_cast <bit_board> (0b1010101001010101101010100101010110101010010101011010101001010101);
		constexpr bit_board light_squares =
		static_cast <bit_board> (0b0101010110101010010101011010101001010101101010100101010110101010);
		
	} /// namespace bb
	
} /// namespace game
