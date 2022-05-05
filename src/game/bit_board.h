// 64-bit bit board

#pragma once

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
		
		namespace rank {
			
			constexpr bit_board r1 =
			static_cast <bit_board> (0b0000000000000000000000000000000000000000000000000000000011111111);
			constexpr bit_board r2 =
			static_cast <bit_board> (0b0000000000000000000000000000000000000000000000001111111100000000);
			constexpr bit_board r3 =
			static_cast <bit_board> (0b0000000000000000000000000000000000000000111111110000000000000000);
			constexpr bit_board r4 =
			static_cast <bit_board> (0b0000000000000000000000000000000011111111000000000000000000000000);
			constexpr bit_board r5 =
			static_cast <bit_board> (0b0000000000000000000000001111111100000000000000000000000000000000);
			constexpr bit_board r6 =
			static_cast <bit_board> (0b0000000000000000111111110000000000000000000000000000000000000000);
			constexpr bit_board r7 =
			static_cast <bit_board> (0b0000000011111111000000000000000000000000000000000000000000000000);
			constexpr bit_board r8 =
			static_cast <bit_board> (0b1111111100000000000000000000000000000000000000000000000000000000);
			
		} /// namespace file
		
		constexpr bit_board queen_side   =  file::fA | file::fB  |  file::fC | file::fD ;
		constexpr bit_board king_side    =  file::fE | file::fF  |  file::fG | file::fH ;
		constexpr bit_board center_files =  file::fC | file::fD  |  file::fE | file::fF ;
		constexpr bit_board center       = (file::fD | file::fE) & (rank::r4 | rank::r5);
		
	} /// namespace bb
	
} /// namespace game
