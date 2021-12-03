#pragma once
#include "ClassicBitBoard.h"
#include <iostream>

namespace chess {
	class SimpleAgent
	{
	public:
		template<bool side>
		static int eval(ClassicBitBoard& brd) {
			int scoreWhite = 0;
			int scoreBlack = 0;
			//Do eval
			//Score white
			uint64_t pawn = brd.WPawn;
			uint64_t rook = brd.WRook;
			uint64_t bishop = brd.WBishop;
			uint64_t knight = brd.WKnight;
			uint64_t queen = brd.WQueen;
			uint64_t king = brd.WKing;
			Bitloop(pawn) {
				scoreWhite += pawnValue[SquareOf(pawn)];
				scoreWhite += 10;
			}
			Bitloop(rook) {
				scoreWhite += rookValue[SquareOf(rook)];
				scoreWhite += 50;
			}
			Bitloop(bishop) {
				scoreWhite += bishopValue[SquareOf(bishop)];
				scoreWhite += 30;
			}
			Bitloop(knight) {
				scoreWhite += knightValue[SquareOf(knight)];
				scoreWhite += 30;
			}
			Bitloop(queen) {
				scoreWhite += queenValue[SquareOf(queen)];
				scoreWhite += 90;
			}
			Bitloop(king) {
				scoreWhite += kingValue[SquareOf(king)];
				scoreWhite += 900;
			}
			//Score black:
			pawn = brd.BPawn;
			rook = brd.BRook;
			bishop = brd.BBishop;
			knight = brd.BKnight;
			queen = brd.BQueen;
			king = brd.BKing;
			Bitloop(pawn) {
				scoreBlack += pawnValue[63 - SquareOf(pawn)];
				scoreBlack += 10;
			}
			Bitloop(rook) {
				scoreBlack += rookValue[63-SquareOf(rook)];
				scoreBlack += 50;
			}
			Bitloop(bishop) {
				scoreBlack += bishopValue[63-SquareOf(bishop)];
				scoreBlack += 30;
			}
			Bitloop(knight) {
				scoreBlack += knightValue[63-SquareOf(knight)];
				scoreBlack += 30;
			}
			Bitloop(queen) {
				scoreBlack += queenValue[63-SquareOf(queen)];
				scoreBlack += 90;
			}
			Bitloop(king) {
				scoreBlack += kingValue[63-SquareOf(king)];
				scoreBlack += 900;
			}

			//Being able to castle is an advantage:
			scoreWhite += (brd.state.WCastleL ? 5 : 0);
			scoreWhite += (brd.state.WCastleR ? 5 : 0);
			scoreBlack += (brd.state.BCastleL ? 5 : 0);
			scoreBlack += (brd.state.BCastleR ? 5 : 0);
			//small bonus for having the right to move
			side ? scoreWhite += 10 : scoreBlack += 10;
			int score = scoreWhite - scoreBlack;
			//std::cout << "Hello from eval" << std::endl;
			//std::cout << "White eval: " << scoreWhite << "\n";
			//std::cout << "black eval: " << scoreBlack << "\n";
			//std::cout << "final aval: " << score << std::endl;
			return score;
		}
	private:
		static constexpr int kingValue[64] = {
			20 , 30, 10,  0,  0, 10, 30, 20,
			20 , 20,  0,  0,  0,  0, 20, 20,
			-10,-20,-20,-20,-20,-20,-20,-10,
			-20,-30,-30,-40,-40,-30,-30,-20,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30,
			-30,-40,-40,-50,-50,-40,-40,-30 
		};
		static constexpr int queenValue[64] = {
			-20,-10,-10, -5, -5,-10,-10,-20,
			-10,  0,  0,  0,  0,  5,  0,-10,
			-10,  0,  5,  5,  5,  5,  5,-10,
			 -5,  0,  5,  5,  5,  5,  0,  0,
			 -5,  0,  5,  5,  5,  5,  0, -5,
			-10,  0,  5,  5,  5,  5,  0,-10,
			-20,-10,-10, -5, -5, -5,-10,-20
		};
		static constexpr int rookValue[64] = {
			  0,  0,  0,  5,  5,  0,  0,  0,
			 -5,  0,  0,  0,  0,  0,  0, -5,
			 -5,  0,  0,  0,  0,  0,  0, -5,
			 -5,  0,  0,  0,  0,  0,  0, -5,
			 -5,  0,  0,  0,  0,  0,  0, -5,
			 -5,  0,  0,  0,  0,  0,  0, -5,
			  5, 10, 10, 10, 10, 10, 10,  5,
			  0,  0,  0,  0,  0,  0,  0,  0
		};
		static constexpr int bishopValue[64] = {
			-20, -10,-10,-10,-10,-10,-10,-20,
			-10,   5,  0,  0,  0,  0,  5,-10,
			-10,  10, 10, 10, 10, 10, 10,-10,
			-10,   0, 10, 10, 10, 10,  0,-10,
			-10,   5,  5, 10, 10,  5,  5,-10,
			-10,   0,  5, 10, 10,  5,  0,-10,
			-10,   0,  0,  0,  0,  0,  0,-10,
			-20, -10,-10,-10,-10,-10,-10,-20
		};
		static constexpr int knightValue[64] = {
			-50, -40,-30,-30,-30,-30,-40,-50,
			-40, -20,  0,  5,  5,  0,-20,-40,
			-30,   5, 10, 15, 15, 10,  5,-30,
			-30,   0, 15, 20, 20, 15,  0,-30,
			-30,   5, 15, 20, 20, 15,  5,-30,
			-30,   0, 10, 15, 15, 10,  0,-30,
			-40, -20,  0,  5,  5,  0,-20,-40,
			-50, -40,-30,-30,-30,-30,-40,-50
		};
		static constexpr int pawnValue[64] = {
			  0,   0,  0,  0,  0,  0,  0,  0,
			  5,  10, 10,-20,-20, 10, 10,  5,
			  5,  -5,-10,  0,  0,-10, -5,  5,
			  0,   0,  0, 20, 20,  0,  0,  0,
			  5,   5, 10, 25, 25, 10,  5,  5,
			 10,  10, 20, 30, 30, 20, 10, 10,
			 50,  50, 50, 50, 50, 50, 50, 50,
			  0,   0,  0,  0,  0,  0,  0,  0
		};

	};
	
	class BetterAgent {

	public:
		template<bool side>
		__forceinline static int eval(ClassicBitBoard& brd)
		{
			int psqt_mg = 0;
			int psqt_eg = 0;
			int piece_val_mg = 0;
			int piece_val_eg = 0;
			int npm = 0;


			uint64_t pawn = brd.WPawn;
			uint64_t rook = brd.WRook;
			uint64_t bishop = brd.WBishop;
			uint64_t knight = brd.WKnight;
			uint64_t queen = brd.WQueen;
			uint64_t king = brd.WKing;

			Bitloop(pawn) {
				psqt_mg += psqt::pawn<true>(SquareOf(pawn));
				psqt_eg += psqt::pawn<false>(SquareOf(pawn));
				piece_val_mg += pieceBonus::pawn<true>();
				piece_val_eg += pieceBonus::pawn<false>();
			}
			Bitloop(rook) {
				psqt_mg += psqt::rook<true>(SquareOf(rook));
				psqt_eg += psqt::rook<false>(SquareOf(rook));
				piece_val_mg += pieceBonus::rook<true>();
				piece_val_eg += pieceBonus::rook<false>();
				npm += pieceBonus::rook<true>();
			}
			Bitloop(bishop) {
				psqt_mg += psqt::bishop<true>(SquareOf(bishop));
				psqt_eg += psqt::bishop<false>(SquareOf(bishop));
				piece_val_mg += pieceBonus::bishop<true>();
				piece_val_eg += pieceBonus::bishop<false>();
				npm += pieceBonus::bishop<true>();
			}
			Bitloop(knight) {
				psqt_mg += psqt::knight<true>(SquareOf(knight));
				psqt_eg += psqt::knight<false>(SquareOf(knight));
				piece_val_mg += pieceBonus::knight<true>();
				piece_val_eg += pieceBonus::knight<false>();
				npm += pieceBonus::knight<true>();
			}
			Bitloop(queen) {
				psqt_mg += psqt::queen<true>(SquareOf(queen));
				psqt_eg += psqt::queen<false>(SquareOf(queen));
				piece_val_mg += pieceBonus::queen<true>();
				piece_val_eg += pieceBonus::queen<false>();
				npm += pieceBonus::queen<true>();
			}
			Bitloop(king) {
				psqt_mg += psqt::king<true>(SquareOf(king));
				psqt_eg += psqt::king<false>(SquareOf(king));
			}
			//Score black:
			pawn = brd.BPawn;
			rook = brd.BRook;
			bishop = brd.BBishop;
			knight = brd.BKnight;
			queen = brd.BQueen;
			king = brd.BKing;
			Bitloop(pawn) {
				psqt_mg -= psqt::pawn<true>(63-SquareOf(pawn));
				psqt_eg -= psqt::pawn<false>(63-SquareOf(pawn));
				piece_val_mg -= pieceBonus::pawn<true>();
				piece_val_eg -= pieceBonus::pawn<false>();
			}
			Bitloop(rook) {
				psqt_mg -= psqt::rook<true>(63-SquareOf(rook));
				psqt_eg -= psqt::rook<false>(63-SquareOf(rook));
				piece_val_mg -= pieceBonus::rook<true>();
				piece_val_eg -= pieceBonus::rook<false>();
				npm -= pieceBonus::rook<true>();
			}
			Bitloop(bishop) {
				psqt_mg -= psqt::bishop<true>(63 - SquareOf(bishop));
				psqt_eg -= psqt::bishop<false>(63 - SquareOf(bishop));
				piece_val_mg -= pieceBonus::bishop<true>();
				piece_val_eg -= pieceBonus::bishop<false>();
				npm -= pieceBonus::bishop<true>();
			}
			Bitloop(knight) {
				psqt_mg -= psqt::knight<true>(63 - SquareOf(knight));
				psqt_eg -= psqt::knight<false>(63 - SquareOf(knight));
				piece_val_mg -= pieceBonus::knight<true>();
				piece_val_eg -= pieceBonus::knight<false>();
				npm -= pieceBonus::knight<true>();
			}
			Bitloop(queen) {
				psqt_mg -= psqt::queen<true>(63 - SquareOf(queen));
				psqt_eg -= psqt::queen<false>(63 - SquareOf(queen));
				piece_val_mg -= pieceBonus::queen<true>();
				piece_val_eg -= pieceBonus::queen<false>();
				npm -= pieceBonus::queen<true>();
			}
			Bitloop(king) {
				psqt_mg -= psqt::king<true>(63 - SquareOf(king));
				psqt_eg -= psqt::king<false>(63 - SquareOf(king));
			}
			//MG eval
			int mg = 0;
			mg += piece_val_mg;
			mg += psqt_mg;

			//EG eval
			int eg = 0;
			eg += piece_val_eg;
			eg += psqt_eg;

			//tempo:
			int tempo = side ? 28 : -28;

			//phase
			int mgLimit = 15258;
			int egLimit = 3915;
			npm = std::max(egLimit, std::min(npm, mgLimit));
			int p = (((npm - egLimit) * 128) / (mgLimit - egLimit)) << 0;

			int evaluation = (((mg * p + ((eg * (128 - p)) << 0)) / 128) << 0);
			evaluation += tempo;
			return evaluation;
		};

		__forceinline int eval(ClassicBitBoard& brd) {
			return (brd.side ? BetterAgent::eval<true>(brd) : BetterAgent::eval<false>(brd));
		}
	private:

		struct pieceBonus {
			template<bool mg>
			_Compiletime int queen() { return mg ? 2538 : 2682; }
			template<bool mg>
			_Compiletime int rook() { return mg ? 1276 : 1380; }
			template<bool mg>
			_Compiletime int bishop() { return mg ? 825 : 915; }
			template<bool mg>
			_Compiletime int knight() { return mg ? 781 : 854; }
			template<bool mg>
			_Compiletime int pawn() { return mg ? 124 : 206; }
		};
		struct psqt {
			// https://github.com/official-stockfish/Stockfish/blob/master/src/psqt.cpp

			static inline int knight_score[2][64]{
				{-175,-92,-74,-73,-73,-74,-92,-175
					- 77,-41,-27,-15,-15,-27,-41, -77,
					-61,-17,  6, 12, 12,  6,-17, -61,
					-35,  8, 40, 49, 49, 40,  8, -35,
					-34, 13, 44, 51, 51, 44, 13, -34,
					-9, 22, 58, 53, 53, 58, 22,  -9,
					-67,-27,  4, 37, 37,  4, 27, -67,
					-201,-83,-56,-26,-26,-56,-83,-201},

				{-96,-65,-49,-21,-21,-49,-65,-96,
					-67,-54,-18, -8, -8,-18,-54,-67,
					-40,-27, -8, 29, 29, -8,-27,-40,
					-35, -2, 13, 28, 28, 13, -2,-35,
					-45,-16,  9, 39, 39,  9,-16,-45,
					-51,-44,-16, 17, 17,-16,-44,-51,
					-69,-50,-51, 12, 12,-51,-50,-69,
				-100,-88,-56,-17,-17,-56,-88,-100}
			};
			static inline int bishop_score[2][64]{
				{ -37, -4, -6,-16,-16, -6, -4,-37,
					-11,  6, 13,  3,  3, 13,  6,-11,
					-5, 15, -4, 12, 12, -4, 15, -5,
					-4,  8, 18, 27, 27, 18,  8, -4,
					-8, 20, 15, 22, 22, 15, 20, -8,
					-11,  4,  1,  8,  8,  1,  4,-11,
					-12,-10,  4,  0,  0,  4,-10,-12,
					-34,  1,-10,-16,-16,-10,  1,-34},

				{-40,-21,-26, -8, -8,-26,-21,-40,
					-26, -9,-12,  1,  1,-12, -9,-26,
					-11, -1, -1,  7,  7, -1, -1,-11,
					-14, -4,  0, 12, 12,  0, -4,-14,
					-12, -1,-10, 11, 11,-10, -1,-12,
					-21,  4,  3,  4,  4,  3,  4,-21,
					-22,-14, -1,  1,  1, -1,-14,-22,
					-32,-29,-26,-17,-17,-26,-29,-32}
			};
			static inline int rook_score[2][64]{
				{ -31,-20,-14, -5, -5,-14,-20,-31,
					-21,-13, -8,  6,  6, -8,-13,-21,
					-25,-11, -1,  3,  3, -1,-11,-25,
					-13, -5, -4, -6, -6, -4, -5,-13,
					-27,-15, -4,  3,  3, -4,-15,-27,
					-22, -2,  6, 12, 12,  6, -2,-22,
					-2, 12, 16, 18, 18, 16, 12, -2,
					-17,-19, -1,  9,  9, -1,-19,-17},

				{ -9,-13,-10, -9, -9,-10,-13, -9,
					-12, -9, -1, -2, -2, -1, -9,-12,
					6, -8, -2, -6, -6, -2, -8,  6,
					-6,  1, -9,  7,  7, -9,  1, -6,
					-5,  8,  7, -6, -6,  7,  8, -5,
					6,  1, -7, 10, 10, -7,  1,  6,
					4,  5, 20, -5, -5, 20,  5,  4,
					18,  0, 19, 13, 13, 19,  0, 18}
			};
			static inline int queen_score[2][64]{
				{  3, -5, -5,  4,  4, -5,  5,  3,
					-3,  5,  8, 12, 12,  8,  5, -3,
					-3,  6, 13,  7,  7, 13,  6, -3,
					4,  5,  9,  8,  8,  9,  5,  4,
					0, 14, 12,  5,  5, 12, 14,  0,
					-4, 10,  6,  8,  8,  6, 10, -4,
					-5,  6, 10,  8,  8, 10,  6, -5,
					-2, -2,  1, -2, -2,  1, -2, -2},

				{-69,-57,-47,-26,-26,-47,-57,-69,
					-54,-31,-22, -4, -4,-22,-31,-54,
					-39,-18, -9,  3,  3, -9,-18,-39,
					-23, -3, 13, 24, 24, 13, -3,-23,
					-29, -6,  9, 21, 21,  9, -6,-29,
					-38,-18,-11,  1,  1,-11,-18,-38,
					-50,-27,-24,  8,  8,-24,-27,-50,
					-74,-52,-43,-34,-34,-43,-52,-74}
			};
			static inline int king_score[2][64]{
				{ 271, 327, 271, 198, 198, 271, 327, 271,
					278, 303, 234, 179, 179, 234, 303, 278,
					195, 258, 169, 120, 120, 169, 258, 195,
					164, 190, 138,  98,  98, 138, 190, 164,
					154, 179, 105,  70,  70, 105, 179, 154,
					123, 145,  81,  31,  31,  81, 145, 123,
					88, 120,  65,  33,  33,  65, 120,  88,
					59,  89,  45,  -1,  -1,  45,  89,  59},

				{   1,  45,  85,  76,  76,  85,  45,   1,
					53, 100, 133, 135, 135, 133, 100,  53,
					88, 130, 169, 175, 175, 169, 130,  88,
					103, 156, 172, 172, 172, 172, 156, 103,
					96, 166, 199, 199, 199, 199, 166,  96,
					92, 172, 184, 191, 191, 184, 172,  92,
					47, 121, 116, 131, 131, 116, 121,  47,
					11,  59,  73,  78,  78,  73,  59,  11}
			};
			static inline int pawn_score[2][64]{
				{  0,  0,  0,  0,  0,  0,  0,  0,
					-3,  9, 21, 16, 18, 11,  4,  2,
					-20,  6, 31, 31, 15, 11,-15, -9,
					-5,  2, 39, 39, 19,  8,-20, -3,
					5,-12, 11, 11,  2,-11, -4, 11,
					11,-14, -8, -8, 22, -6,-11,  3,
					-9, 10,  4,  4,-11, -2,  6, -7,
					0,  0,  0,  0,  0,  0,  0,  0},

				{  0,  0,  0,  0,  0,  0,  0,  0,
					-18, -6,  6, 16,  5,  9, -6, -8,
					-5, -8,  3,  2,  5,-10, -7, -9,
					-6,-11,-13,-14, -2, -8,  1,  7,
					9, 14, -4, -5, -6,  2,  6, 12,
					14,  8,  9, 30, 29, 19, 18, 27,
					7,  7, 17, 24, 22, 13,-14, -1,
					0,  0,  0,  0,  0,  0,  0,  0}
			};



			template<bool mg>
			_Compiletime int knight(square sq) {
				if (mg) {
					return (knight_score[0][sq]);
				}
				else {
					return (knight_score[1][sq]);
				}
			}
			template<bool mg>
			_Compiletime int bishop(square sq) {
				if (mg) {
					return (bishop_score[0][sq]);
				}
				else {
					return (bishop_score[1][sq]);
				}
			}
			template<bool mg>
			_Compiletime int rook(square sq) {
				if (mg) {
					return (rook_score[0][sq]);
				}
				else {
					return (rook_score[1][sq]);
				}
			}
			template<bool mg>
			_Compiletime int queen(square sq) {
				if (mg) {
					return (queen_score[0][sq]);
				}
				else {
					return (queen_score[1][sq]);
				}
			}
			template<bool mg>
			_Compiletime int king(square sq) {
				if (mg) {
					return (king_score[0][sq]);
				}
				else {
					return (king_score[1][sq]);
				}
			}
			template<bool mg>
			_Compiletime int pawn(square sq) {
				if (mg) {
					return (pawn_score[0][sq]);
				}
				else {
					return (pawn_score[1][sq]);
				}
			}


		};

	
	};
}

