#pragma once
#include "../chess/ClassicBitBoard.h"
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
			int imbalance_total_mg = 0;
			int imbalance_total_eg = 0;
			int npm = 0;
            int pieceCount[2][7]= {0};

			bool W_K = false;
			bool B_K = false;
			bool W_KN = false;
			bool B_KN = false;
			bool W_KB = false;
			bool B_KB = false;
			uint8_t wBishop = 0;
            uint8_t bBishop = 0;
            uint8_t wKnight = 0;
            uint8_t bKnight = 0;

			uint64_t pawn = brd.WPawn;
			uint64_t rook = brd.WRook;
			uint64_t bishop = brd.WBishop;
			uint64_t knight = brd.WKnight;
			uint64_t queen = brd.WQueen;
			uint64_t king = brd.WKing;

			Bitloop(pawn) {
                pieceCount[0][1]++;
				psqt_mg += Psqt::pawn<true>(SquareOf(pawn));
				psqt_eg += Psqt::pawn<false>(SquareOf(pawn));
				piece_val_mg += PieceBonus::pawn<true>();
				piece_val_eg += PieceBonus::pawn<false>();
			}
			Bitloop(rook) {
                pieceCount[0][4]++;
				psqt_mg += Psqt::rook<true>(SquareOf(rook));
				psqt_eg += Psqt::rook<false>(SquareOf(rook));
				piece_val_mg += PieceBonus::rook<true>();
				piece_val_eg += PieceBonus::rook<false>();
				npm += PieceBonus::rook<true>();
			}
			Bitloop(bishop) {
                wBishop++;
                pieceCount[0][3]++;
				psqt_mg += Psqt::bishop<true>(SquareOf(bishop));
				psqt_eg += Psqt::bishop<false>(SquareOf(bishop));
				piece_val_mg += PieceBonus::bishop<true>();
				piece_val_eg += PieceBonus::bishop<false>();
				npm += PieceBonus::bishop<true>();
			}
            if (pieceCount[0][3] >= 2){ //Bishop pair
                pieceCount[0][0]++;
                //imbalance_total_mg += Imbalance::bishopPair<true>();
                //imbalance_total_eg += Imbalance::bishopPair<false>();
            }
			Bitloop(knight) {
                wKnight++;
                pieceCount[0][2]++;
				psqt_mg += Psqt::knight<true>(SquareOf(knight));
				psqt_eg += Psqt::knight<false>(SquareOf(knight));
				piece_val_mg += PieceBonus::knight<true>();
				piece_val_eg += PieceBonus::knight<false>();
				npm += PieceBonus::knight<true>();
			}
			Bitloop(queen) {
                pieceCount[0][5]++;
				psqt_mg += Psqt::queen<true>(SquareOf(queen));
				psqt_eg += Psqt::queen<false>(SquareOf(queen));
				piece_val_mg += PieceBonus::queen<true>();
				piece_val_eg += PieceBonus::queen<false>();
				npm += PieceBonus::queen<true>();
			}
			Bitloop(king) {
                pieceCount[0][6]++;
				psqt_mg += Psqt::king<true>(SquareOf(king));
				psqt_eg += Psqt::king<false>(SquareOf(king));
			}
            //Insufficient material?
			if (!brd.WPawn && !brd.WRook && !brd.WBishop && !brd.WKnight && !brd.WQueen){
                W_K = true; //lone king
			}
			if (!brd.WPawn && !brd.WRook && !brd.WBishop && wKnight == 1 && !brd.WQueen){
                W_KN = true; // only king and knight
			}
			if (!brd.WPawn && !brd.WRook && wBishop == 1 && !brd.WKnight && !brd.WQueen){
                W_KB = true; // only king and bishop
			}
			//Score black:
			pawn = brd.BPawn;
			rook = brd.BRook;
			bishop = brd.BBishop;
			knight = brd.BKnight;
			queen = brd.BQueen;
			king = brd.BKing;
			Bitloop(pawn) {
                pieceCount[1][1]++;
				psqt_mg -= Psqt::pawn<true>(63 - SquareOf(pawn));
				psqt_eg -= Psqt::pawn<false>(63 - SquareOf(pawn));
				piece_val_mg -= PieceBonus::pawn<true>();
				piece_val_eg -= PieceBonus::pawn<false>();
			}
			Bitloop(rook) {
                pieceCount[1][4]++;
				psqt_mg -= Psqt::rook<true>(63 - SquareOf(rook));
				psqt_eg -= Psqt::rook<false>(63 - SquareOf(rook));
				piece_val_mg -= PieceBonus::rook<true>();
				piece_val_eg -= PieceBonus::rook<false>();
				npm -= PieceBonus::rook<true>();
			}
			Bitloop(bishop) {
                bBishop++;
                pieceCount[1][3]++;
				psqt_mg -= Psqt::bishop<true>(63 - SquareOf(bishop));
				psqt_eg -= Psqt::bishop<false>(63 - SquareOf(bishop));
				piece_val_mg -= PieceBonus::bishop<true>();
				piece_val_eg -= PieceBonus::bishop<false>();
				npm -= PieceBonus::bishop<true>();
			}
            if (pieceCount[1][3] >= 2){ //Bishop pair
                pieceCount[1][0]++;
                //imbalance_total_mg -= Imbalance::bishopPair<true>();
                //imbalance_total_eg -= Imbalance::bishopPair<false>();
            }
			Bitloop(knight) {
                bKnight++;
                pieceCount[1][2]++;
				psqt_mg -= Psqt::knight<true>(63 - SquareOf(knight));
				psqt_eg -= Psqt::knight<false>(63 - SquareOf(knight));
				piece_val_mg -= PieceBonus::knight<true>();
				piece_val_eg -= PieceBonus::knight<false>();
				npm -= PieceBonus::knight<true>();
			}
			Bitloop(queen){
                pieceCount[1][5]++;
				psqt_mg -= Psqt::queen<true>(63 - SquareOf(queen));
				psqt_eg -= Psqt::queen<false>(63 - SquareOf(queen));
				piece_val_mg -= PieceBonus::queen<true>();
				piece_val_eg -= PieceBonus::queen<false>();
				npm -= PieceBonus::queen<true>();
			}
			Bitloop(king) {
                pieceCount[1][6]++;
				psqt_mg -= Psqt::king<true>(63 - SquareOf(king));
				psqt_eg -= Psqt::king<false>(63 - SquareOf(king));
			}

            //Insufficient material?
            if (!brd.BPawn && !brd.BRook && !brd.BBishop && !brd.BKnight && !brd.BQueen){
                B_K = true; //lone king
            }
            if (!brd.BPawn && !brd.BRook && !brd.BBishop && bKnight == 1 && !brd.BQueen){
                B_KN = true; // only king and knight
            }
            if (!brd.BPawn && !brd.BRook && bBishop == 1 && !brd.BKnight && !brd.BQueen){
                B_KB = true; // only king and bishop
            }

			//Insufficient material?
            if ( (W_K || W_KB || W_KN) &&
                 (B_K || B_KB || B_KN)){
                return 0; //DRAW due to insufficient material
            }

            //Imbalance //TODO: check if the implementation is correct
            // Source: https://hxim.github.io/Stockfish-Evaluation-Guide/ && https://github.com/official-stockfish/Stockfish/blob/master/src/material.cpp
            imbalance_total_mg = Imbalance::imbalance<side,true>(pieceCount) - Imbalance::imbalance<!side,true>(pieceCount);
            imbalance_total_eg = Imbalance::imbalance<side,false>(pieceCount) - Imbalance::imbalance<!side,false>(pieceCount);
            /*for (int i = 0; i < 2; i++){
                for (int j = 0; j < 6; j++) {
                    std::cout << pieceCount[i][j] << " ";
                }
                std::cout << "\n";
            }*/

			//MG eval
			int mg = 0;
			mg += piece_val_mg;
			mg += psqt_mg;
            mg += imbalance_total_mg/16;

			//EG eval
			int eg = 0;
			eg += piece_val_eg;
			eg += psqt_eg;
			eg += imbalance_total_eg/16;

			//std::cout << "imbalance mg: " <<imbalance_total_mg;
            //std::cout << " imbalance eg: " <<imbalance_total_eg << std::endl;
            //std::cout << "mg: " <<mg;
            //std::cout << " eg: " <<eg<< std::endl;

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

		struct PieceBonus {
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
		struct Psqt {
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

        struct Imbalance{
            template <bool mg>
            _Compiletime int bishopPair(){return mg ? 1419 : 1455;}

            static inline int quadraticOursMG[][8]{
                //mg
                {1419},
                {101,37},
                {57,249,-49},
                {0,118,10,0},
                {-63,-5,100,132,-246},
                {-210,37,147,161,-158,-9}
            };
            static inline int quadraticOursEG[][8]{
                    //mg
                    {1455},
                    {28,39},
                    {64,187,-62},
                    {0,137,27,0},
                    {-68,3,81,118,-244},
                    {-211,14,141,105,-174,-31}
            };
            static inline int quadraticTheirsMG[][8]{
                    //mg
                    {},
                    {33},
                    {46,106},
                    {75,59,60},
                    {26,6,38,-12},
                    {97,100,-58,112,276}
            };
            static inline int quadraticTheirsEG[][8]{
                    //eg
                    {},
                    {30},
                    {18,84},
                    {35,44,15},
                    {35,22,39,-2},
                    {93,163,-91,192,225}
            };
            template<bool mg>
            _Compiletime int quadraticOurs(int p1, int p2) {
                if (mg) {
                    return (quadraticOursMG[p1][p2]);
                }
                else {
                    return (quadraticOursEG[p1][p2]);
                }
            }
            template<bool mg>
            _Compiletime int quadraticTheirs(int p1, int p2) {
                if (mg) {
                    return (quadraticTheirsMG[p1][p2]);
                }
                else {
                    return (quadraticTheirsEG[p1][p2]);
                }
            }
            template <bool side, bool mg>
            _Compiletime int imbalance( int pieceCount[2][7]){
                constexpr int us = side?0:1;
                constexpr int them = side?1:0;
                int bonus = 0;
                for (int p1 = 0; p1 < 6; p1 ++){ //0 = bishopPair;
                    if (!pieceCount[us][p1]) continue;
                    int v = quadraticOurs<mg>(p1,p1) * pieceCount[us][p1];
                    for (int p2 = 0; p2 < p1; p2++){
                        v += quadraticOurs<mg>(p1,p2) * pieceCount[us][p2]+
                                quadraticTheirs<mg>(p1,p2) * pieceCount[them][p2];
                    }
                    bonus += pieceCount[us][p1] * v;
                }
                return bonus;
            }
        };

	
	};
}

