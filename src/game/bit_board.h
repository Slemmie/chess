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
			
			constexpr bit_board fA =
			static_cast <bit_board> (0b0000000100000001000000010000000100000001000000010000000100000001);
			constexpr bit_board fB =
			static_cast <bit_board> (0b0000001000000010000000100000001000000010000000100000001000000010);
			constexpr bit_board fC =
			static_cast <bit_board> (0b0000010000000100000001000000010000000100000001000000010000000100);
			constexpr bit_board fD =
			static_cast <bit_board> (0b0000100000001000000010000000100000001000000010000000100000001000);
			constexpr bit_board fE =
			static_cast <bit_board> (0b0001000000010000000100000001000000010000000100000001000000010000);
			constexpr bit_board fF =
			static_cast <bit_board> (0b0010000000100000001000000010000000100000001000000010000000100000);
			constexpr bit_board fG =
			static_cast <bit_board> (0b0100000001000000010000000100000001000000010000000100000001000000);
			constexpr bit_board fH =
			static_cast <bit_board> (0b1000000010000000100000001000000010000000100000001000000010000000);
			
		} /// namespace file
		
	} /// namespace bb
	
} /// namespace game
