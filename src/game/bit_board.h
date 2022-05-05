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
		
		namespace file {
			
			constexpr bit_board A =
			static_cast <bit_board> (0b0000000100000001000000010000000100000001000000010000000100000001);
			constexpr bit_board B =
			static_cast <bit_board> (0b0000001000000010000000100000001000000010000000100000001000000010);
			constexpr bit_board C =
			static_cast <bit_board> (0b0000010000000100000001000000010000000100000001000000010000000100);
			constexpr bit_board D =
			static_cast <bit_board> (0b0000100000001000000010000000100000001000000010000000100000001000);
			constexpr bit_board E =
			static_cast <bit_board> (0b0001000000010000000100000001000000010000000100000001000000010000);
			constexpr bit_board F =
			static_cast <bit_board> (0b0010000000100000001000000010000000100000001000000010000000100000);
			constexpr bit_board G =
			static_cast <bit_board> (0b0100000001000000010000000100000001000000010000000100000001000000);
			constexpr bit_board H =
			static_cast <bit_board> (0b1000000010000000100000001000000010000000100000001000000010000000);
			
		} /// namespace file
		
	} /// namespace bb
	
} /// namespace game
