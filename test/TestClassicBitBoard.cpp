#include "../src/chess/ClassicBitBoard.h"
#include <algorithm>
#include <string>
#include "../src/SearchAgents.h"
#include "../src/EvalAgents.h"
#include "../src/TranspositionTable.h"
#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

namespace ClassicBitBoard_Test {
	#pragma region Move
	struct MoveEncodings;
	chess::bit ToBit(std::string sq);
	struct MoveTest : public testing::TestWithParam<std::vector<MoveEncodings>> {};

	struct MoveEncodings {
		std::string SAN;
		std::string LAN;
		chess::Move move;
		std::string fen;
		MoveEncodings(std::string san, std::string lan, chess::Move move,std::string fen) : SAN(san), LAN(lan), move(move),fen(fen) {}
		friend std::ostream& operator<<(std::ostream& os, const MoveEncodings& move) {
			os << move.LAN;
			return os;
		}
		std::string to_string() const {
			return LAN;
		}
	};
	chess::bit ToBit(std::string sq) {
		char file = sq[0];
		char rank = sq[1];
		int col = (int)std::string("hgfedcba").find(file);
	    int row = (int)std::string("12345678").find(rank);
		return 1ull << (row * 8 + col);
	}

	TEST_P(MoveTest, Move2San) {
		for (int i = 0; i < GetParam().size(); i++) {
			std::string fen = GetParam()[i].fen;
			chess::Move move = GetParam()[i].move;
			std::string san = GetParam()[i].SAN;

			if (fen == "startpos") {
				fen = chess::ClassicBitBoard::startpos;
			}
			if (fen == "startpos_black") {
				fen = chess::ClassicBitBoard::startpos;
				std::replace(fen.begin(), fen.end(), 'w', 'b');
			}
			chess::ClassicBitBoard board(fen);
			EXPECT_EQ(san, board.Move2SAN(move)) << i << ":" << fen; 

			std::string result;
			EXPECT_TRUE(board.Move2SAN(move, result));
			EXPECT_EQ(san, result) << i << ":" << fen;
		
		}
		
	}
	TEST_P(MoveTest, Move2Lan) {
		for (int i = 0; i < GetParam().size(); i++) {
			std::string fen = GetParam()[i].fen;
			chess::Move move = GetParam()[i].move;
			std::string lan = GetParam()[i].LAN;

			if (fen == "startpos") {
				fen = chess::ClassicBitBoard::startpos;
			}
			if (fen == "startpos_black") {
				fen = chess::ClassicBitBoard::startpos;
				std::replace(fen.begin(), fen.end(), 'w', 'b');
			}
			chess::ClassicBitBoard board(fen);
			EXPECT_EQ(lan, board.Move2LAN(move)) << i << ":" << fen;
			
			std::string result;
			EXPECT_TRUE(board.Move2LAN(move, result));
			EXPECT_EQ(lan, result) << i << ":" << fen;
		}
	}
	TEST_P(MoveTest, San2Move) {
		for (int i = 0; i < GetParam().size(); i++) {
			std::string fen = GetParam()[i].fen;
			chess::Move move = GetParam()[i].move;
			std::string san = GetParam()[i].SAN;

			if (fen == "startpos") {
				fen = chess::ClassicBitBoard::startpos;
			}
			if (fen == "startpos_black") {
				fen = chess::ClassicBitBoard::startpos;
				std::replace(fen.begin(), fen.end(), 'w', 'b');
			}
			chess::ClassicBitBoard board(fen);
			EXPECT_EQ(move, board.SAN2Move(san)) << i << ":" << fen;
			chess::Move result = board.SAN2Move(san);
			EXPECT_EQ(move.IsWhite, result.IsWhite);
			EXPECT_EQ(move.flags, result.flags);
			EXPECT_EQ(move.to, result.to);
			EXPECT_EQ(move.from, result.from);
		
			result = chess::Move();
			EXPECT_TRUE(board.SAN2Move(san, result));
			EXPECT_EQ(move, result) << i << ":" << fen;
			EXPECT_EQ(move.IsWhite, result.IsWhite);
			EXPECT_EQ(move.flags, result.flags);
			EXPECT_EQ(move.to, result.to);
			EXPECT_EQ(move.from, result.from);
		}
	}
	TEST_P(MoveTest, San2Lan) {
		for (int i = 0; i < GetParam().size(); i++) {
			std::string fen = GetParam()[i].fen;
			std::string lan = GetParam()[i].LAN;
			std::string san = GetParam()[i].SAN;

			if (fen == "startpos") {
				fen = chess::ClassicBitBoard::startpos;
			}
			if (fen == "startpos_black") {
				fen = chess::ClassicBitBoard::startpos;
				std::replace(fen.begin(), fen.end(), 'w', 'b');
			}
			chess::ClassicBitBoard board(fen);
			chess::Move move = board.SAN2Move(san);
			EXPECT_EQ(lan, board.Move2LAN(move)) << i << ":" << fen;
			
			chess::Move m;
			std::string result;
			EXPECT_TRUE(board.SAN2Move(san, m));
			EXPECT_TRUE(board.Move2LAN(m, result));
			EXPECT_EQ(lan, result) << i << ":" << fen;
		}
	}
	TEST_P(MoveTest, Lan2Move) {
		for (int i = 0; i < GetParam().size(); i++) {
			std::string fen = GetParam()[i].fen;
			chess::Move move = GetParam()[i].move;
			std::string lan = GetParam()[i].LAN;

			if (fen == "startpos") {
				fen = chess::ClassicBitBoard::startpos;
			}
			if (fen == "startpos_black") {
				fen = chess::ClassicBitBoard::startpos;
				std::replace(fen.begin(), fen.end(), 'w', 'b');
			}
			chess::ClassicBitBoard board(fen);
			EXPECT_EQ(move, board.LAN2Move(lan)) << i << ":" << fen;
			chess::Move result = board.LAN2Move(lan);
			EXPECT_EQ(move.IsWhite, result.IsWhite);
			EXPECT_EQ(move.flags, result.flags);
			EXPECT_EQ(move.to, result.to);
			EXPECT_EQ(move.from, result.from);

			result = chess::Move();
			EXPECT_TRUE(board.LAN2Move(lan, result));
			EXPECT_EQ(move, result) << i << ":" << fen;
			EXPECT_EQ(move.IsWhite, result.IsWhite);
			EXPECT_EQ(move.flags, result.flags);
			EXPECT_EQ(move.to, result.to);
			EXPECT_EQ(move.from, result.from);
		}
		
		
	}
	TEST_P(MoveTest, Lan2San) {
		for (int i = 0; i < GetParam().size(); i++) {
			std::string fen = GetParam()[i].fen;
			std::string lan = GetParam()[i].LAN;
			std::string san = GetParam()[i].SAN;

			if (fen == "startpos") {
				fen = chess::ClassicBitBoard::startpos;
			}
			if (fen == "startpos_black") {
				fen = chess::ClassicBitBoard::startpos;
				std::replace(fen.begin(), fen.end(), 'w', 'b');
			}
			chess::ClassicBitBoard board(fen);
			chess::Move move = board.LAN2Move(lan);
			EXPECT_EQ(san, board.Move2SAN(move)) << i << ":" << fen;

			chess::Move m;
			std::string result;
			EXPECT_TRUE(board.LAN2Move(lan, m));
			EXPECT_TRUE(board.Move2SAN(m, result));
			EXPECT_EQ(san, result) << i << ":" << fen;
		}
	}
	TEST_P(MoveTest, FromUCI) {
		for (int i = 0; i < GetParam().size(); i++) {
			//Note: = lan 2 move!
			std::string fen = GetParam()[i].fen;
			chess::Move move = GetParam()[i].move;
			std::string lan = GetParam()[i].LAN;

			if (fen == "startpos") {
				fen = chess::ClassicBitBoard::startpos;
			}
			else if (fen == "startpos_black") {
				fen = chess::ClassicBitBoard::startpos;
				std::replace(fen.begin(), fen.end(), 'w', 'b');
			}
			chess::ClassicBitBoard board(fen);
			EXPECT_EQ(move, board.movefromUCI(lan)) << i << ":" << fen;
			chess::Move result = board.movefromUCI(lan);
			EXPECT_EQ(move.flags, result.flags);
			EXPECT_EQ(move.to, result.to);
			EXPECT_EQ(move.from, result.from);

			result = chess::Move();
			EXPECT_TRUE(board.movefromUCI(lan, result));
			EXPECT_EQ(move, result) << i << ":" << fen;
			EXPECT_EQ(move.IsWhite, result.IsWhite);
			EXPECT_EQ(move.flags, result.flags);
			EXPECT_EQ(move.to, result.to);
			EXPECT_EQ(move.from, result.from);
		}
	}

	std::vector<std::vector<MoveEncodings>> testMoves = {
			//Non ambiguating moves
			//starpos pawn moves (1 square) white
			{ //0
				MoveEncodings("a3","a2a3",chess::Move(true,ToBit("a2"),ToBit("a3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("b3","b2b3",chess::Move(true,ToBit("b2"),ToBit("b3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("c3","c2c3",chess::Move(true,ToBit("c2"),ToBit("c3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("d3","d2d3",chess::Move(true,ToBit("d2"),ToBit("d3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("e3","e2e3",chess::Move(true,ToBit("e2"),ToBit("e3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("f3","f2f3",chess::Move(true,ToBit("f2"),ToBit("f3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("g3","g2g3",chess::Move(true,ToBit("g2"),ToBit("g3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("h3","h2h3",chess::Move(true,ToBit("h2"),ToBit("h3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				//starpos pawn moves (2 squares) white
				MoveEncodings("a4","a2a4",chess::Move(true,ToBit("a2"),ToBit("a4"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("b4","b2b4",chess::Move(true,ToBit("b2"),ToBit("b4"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("c4","c2c4",chess::Move(true,ToBit("c2"),ToBit("c4"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("d4","d2d4",chess::Move(true,ToBit("d2"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("e4","e2e4",chess::Move(true,ToBit("e2"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("f4","f2f4",chess::Move(true,ToBit("f2"),ToBit("f4"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("g4","g2g4",chess::Move(true,ToBit("g2"),ToBit("g4"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				MoveEncodings("h4","h2h4",chess::Move(true,ToBit("h2"),ToBit("h4"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos"),
				//starpos knight moves white
				MoveEncodings("Nc3","b1c3",chess::Move(true,ToBit("b1"),ToBit("c3"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"startpos"),
				MoveEncodings("Na3","b1a3",chess::Move(true,ToBit("b1"),ToBit("a3"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"startpos"),
				MoveEncodings("Nf3","g1f3",chess::Move(true,ToBit("g1"),ToBit("f3"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"startpos"),
				MoveEncodings("Nh3","g1h3",chess::Move(true,ToBit("g1"),ToBit("h3"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"startpos")
			},

				//starpos pawn moves (1 square) black
			{ //1
				MoveEncodings("a6","a7a6",chess::Move(false,ToBit("a7"),ToBit("a6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("b6","b7b6",chess::Move(false,ToBit("b7"),ToBit("b6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("c6","c7c6",chess::Move(false,ToBit("c7"),ToBit("c6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("d6","d7d6",chess::Move(false,ToBit("d7"),ToBit("d6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("e6","e7e6",chess::Move(false,ToBit("e7"),ToBit("e6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("f6","f7f6",chess::Move(false,ToBit("f7"),ToBit("f6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("g6","g7g6",chess::Move(false,ToBit("g7"),ToBit("g6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("h6","h7h6",chess::Move(false,ToBit("h7"),ToBit("h6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				//starpos pawn moves (2 squares) black
				MoveEncodings("a5","a7a5",chess::Move(false,ToBit("a7"),ToBit("a5"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("b5","b7b5",chess::Move(false,ToBit("b7"),ToBit("b5"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("c5","c7c5",chess::Move(false,ToBit("c7"),ToBit("c5"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("d5","d7d5",chess::Move(false,ToBit("d7"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("e5","e7e5",chess::Move(false,ToBit("e7"),ToBit("e5"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("f5","f7f5",chess::Move(false,ToBit("f7"),ToBit("f5"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("g5","g7g5",chess::Move(false,ToBit("g7"),ToBit("g5"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				MoveEncodings("h5","h7h5",chess::Move(false,ToBit("h7"),ToBit("h5"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"startpos_black"),
				//starpos knight moves black
				MoveEncodings("Nc6","b8c6",chess::Move(false,ToBit("b8"),ToBit("c6"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"startpos_black"),
				MoveEncodings("Na6","b8a6",chess::Move(false,ToBit("b8"),ToBit("a6"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"startpos_black"),
				MoveEncodings("Nf6","g8f6",chess::Move(false,ToBit("g8"),ToBit("f6"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"startpos_black"),
				MoveEncodings("Nh6","g8h6",chess::Move(false,ToBit("g8"),ToBit("h6"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"startpos_black")
			},

			//Castling
			{ //2
				MoveEncodings("O-O","e1g1",chess::Move(true,ToBit("e1"),ToBit("g1"), encode_flags(8,chess::BoardPiece::bp_King,0)),"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"),
				MoveEncodings("O-O-O","e1c1",chess::Move(true,ToBit("e1"),ToBit("c1"), encode_flags(4,chess::BoardPiece::bp_King,0)),"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"),
				MoveEncodings("O-O","e8g8",chess::Move(false,ToBit("e8"),ToBit("g8"), encode_flags(8,chess::BoardPiece::bp_King,0)),"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"),
				MoveEncodings("O-O-O","e8c8",chess::Move(false,ToBit("e8"),ToBit("c8"), encode_flags(4,chess::BoardPiece::bp_King,0)),"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1")},
		
			//Capturing
			{ //3
				MoveEncodings("Bxa6","e2a6",chess::Move(true,ToBit("e2"),ToBit("a6"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"),
				MoveEncodings("bxc3","b4c3",chess::Move(false,ToBit("b4"),ToBit("c3"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"),
				MoveEncodings("dxe6","d5e6",chess::Move(true,ToBit("d5"),ToBit("e6"), encode_flags(0,chess::BoardPiece::bp_Pawn,0)),"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1")
			},
		
			//promotion
			{ //4
				// No capture
				MoveEncodings("b8=Q","b7b8q",chess::Move(true,ToBit("b7"),ToBit("b8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Queen)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("b8=R","b7b8r",chess::Move(true,ToBit("b7"),ToBit("b8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Rook)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("b8=N","b7b8n",chess::Move(true,ToBit("b7"),ToBit("b8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Knight)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("b8=B","b7b8b",chess::Move(true,ToBit("b7"),ToBit("b8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Bishop)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("g1=Q","g2g1q",chess::Move(false,ToBit("g2"),ToBit("g1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Queen)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("g1=R","g2g1r",chess::Move(false,ToBit("g2"),ToBit("g1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Rook)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("g1=N","g2g1n",chess::Move(false,ToBit("g2"),ToBit("g1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Knight)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("g1=B","g2g1b",chess::Move(false,ToBit("g2"),ToBit("g1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Bishop)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				// Capture left
				MoveEncodings("bxa8=Q","b7a8q",chess::Move(true,ToBit("b7"),ToBit("a8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Queen)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("bxa8=R","b7a8r",chess::Move(true,ToBit("b7"),ToBit("a8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Rook)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("bxa8=N","b7a8n",chess::Move(true,ToBit("b7"),ToBit("a8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Knight)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("bxa8=B","b7a8b",chess::Move(true,ToBit("b7"),ToBit("a8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Bishop)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("gxf1=Q","g2f1q",chess::Move(false,ToBit("g2"),ToBit("f1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Queen)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("gxf1=R","g2f1r",chess::Move(false,ToBit("g2"),ToBit("f1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Rook)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("gxf1=N","g2f1n",chess::Move(false,ToBit("g2"),ToBit("f1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Knight)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("gxf1=B","g2f1b",chess::Move(false,ToBit("g2"),ToBit("f1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Bishop)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				// Capture right
				MoveEncodings("bxc8=Q","b7c8q",chess::Move(true,ToBit("b7"),ToBit("c8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Queen)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("bxc8=R","b7c8r",chess::Move(true,ToBit("b7"),ToBit("c8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Rook)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("bxc8=N","b7c8n",chess::Move(true,ToBit("b7"),ToBit("c8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Knight)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("bxc8=B","b7c8b",chess::Move(true,ToBit("b7"),ToBit("c8"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Bishop)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N w - - 0 1"),
				MoveEncodings("gxh1=Q","g2h1q",chess::Move(false,ToBit("g2"),ToBit("h1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Queen)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("gxh1=R","g2h1r",chess::Move(false,ToBit("g2"),ToBit("h1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Rook)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("gxh1=N","g2h1n",chess::Move(false,ToBit("g2"),ToBit("h1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Knight)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1"),
				MoveEncodings("gxh1=B","g2h1b",chess::Move(false,ToBit("g2"),ToBit("h1"), encode_flags(1,chess::BoardPiece::bp_Pawn,chess::BoardPiece::bp_Bishop)),"n1n5/PPP2k2/8/8/8/8/2K2ppp/5N1N b - - 0 1")			
			},
		
			//check
			{ //5
				MoveEncodings("Qb4+","e7b4",chess::Move(false,ToBit("e7"),ToBit("b4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"r3k2r/p1ppqpb1/bn2pnp1/1N1PN3/4P3/1p2BQ1p/PPP1BPPP/R3K2R b KQkq - 1 2"),
				MoveEncodings("Nxc7+","b5c7",chess::Move(true,ToBit("b5"),ToBit("c7"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"r3k2r/p1ppqpb1/bn2pnp1/1N1PN3/4P3/1p2BQ1p/PPP1BPPP/R3K2R w KQkq - 1 2")
			},

			//checkmate
			{ //6
				MoveEncodings("Re8#","e5e8",chess::Move(true,ToBit("e5"),ToBit("e8"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"6k1/8/6K1/4R3/8/8/8/8 w - - 0 1"),
				MoveEncodings("Re8#","e5e8",chess::Move(false,ToBit("e5"),ToBit("e8"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"6K1/8/6k1/4r3/8/8/8/8 b - - 0 1"),
				MoveEncodings("Rxe8#","e5e8",chess::Move(true,ToBit("e5"),ToBit("e8"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"4r1k1/8/6K1/4R3/8/8/8/8 w - - 0 1"),
				MoveEncodings("Rxe8#","e5e8",chess::Move(false,ToBit("e5"),ToBit("e8"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"4R1K1/8/6k1/4r3/8/8/8/8 b - - 0 1")
			},
			// enpassant
			{ //7
				MoveEncodings("exf6","e5f6",chess::Move(true,ToBit("e5"),ToBit("f6"), encode_flags(2,chess::BoardPiece::bp_Pawn,0)),"rnbqkb1r/ppppp1pp/7n/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3"),
				MoveEncodings("axb3","a4b3",chess::Move(false,ToBit("a4"),ToBit("b3"), encode_flags(2,chess::BoardPiece::bp_Pawn,0)),"rnbqkbnr/3ppppp/8/1pp5/pP2P3/2PP1NP1/P4P1P/RNBQKB1R b KQkq b3 0 6"),
			},


			/* ambiguating moves */
			//Normal moves
			{// 8
				MoveEncodings("Bce5","c7e5",chess::Move(true,ToBit("c7"),ToBit("e5"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 w - - 0 1"),
				MoveEncodings("Bge5","g7e5",chess::Move(true,ToBit("g7"),ToBit("e5"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 w - - 0 1"),
				MoveEncodings("Nde4","d2e4",chess::Move(true,ToBit("d2"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 w - - 0 1"),
				MoveEncodings("Nfe4","f2e4",chess::Move(true,ToBit("f2"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 w - - 0 1"),
				MoveEncodings("Rbd4","b4d4",chess::Move(true,ToBit("b4"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 w - - 0 1"),
				MoveEncodings("Rgd4","g4d4",chess::Move(true,ToBit("g4"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 w - - 0 1"),
				MoveEncodings("Q2e4","e2e4",chess::Move(true,ToBit("e2"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 w - - 0 1"),
				MoveEncodings("Q7e4","e7e4",chess::Move(true,ToBit("e7"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 w - - 0 1"),
				
				MoveEncodings("Bce4","c2e4",chess::Move(false,ToBit("c2"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 b - - 0 1"),
				MoveEncodings("Bge4","g2e4",chess::Move(false,ToBit("g2"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 b - - 0 1"),
				MoveEncodings("Nde5","d7e5",chess::Move(false,ToBit("d7"),ToBit("e5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 b - - 0 1"),
				MoveEncodings("Nfe5","f7e5",chess::Move(false,ToBit("f7"),ToBit("e5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 b - - 0 1"),
				MoveEncodings("Rbd5","b5d5",chess::Move(false,ToBit("b5"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 b - - 0 1"),
				MoveEncodings("Rgd5","g5d5",chess::Move(false,ToBit("g5"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"6k1/2BnQnB1/8/1r4r1/1R4R1/8/2bNQNb1/6K1 b - - 0 1"),
				MoveEncodings("Q2e4","e2e4",chess::Move(false,ToBit("e2"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"6k1/2BnqnB1/8/1r4r1/1R4R1/8/2bNqNb1/6K1 b - - 0 1"),
				MoveEncodings("Q7e4","e7e4",chess::Move(false,ToBit("e7"),ToBit("e4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"6k1/2BnqnB1/8/1r4r1/1R4R1/8/2bNqNb1/6K1 b - - 0 1"),
			},
			// captures
			{ // 9
				MoveEncodings("Rbxd5","b5d5",chess::Move(true,ToBit("b5"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Rfxd5","f5d5",chess::Move(true,ToBit("f5"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("R3xd5","d3d5",chess::Move(true,ToBit("d3"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("R7xd5","d7d5",chess::Move(true,ToBit("d7"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Rook,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				
				MoveEncodings("Ne3xd5","e3d5",chess::Move(true,ToBit("e3"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Nc3xd5","c3d5",chess::Move(true,ToBit("c3"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Nb4xd5","b4d5",chess::Move(true,ToBit("b4"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Nb6xd5","b6d5",chess::Move(true,ToBit("b6"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Nc7xd5","c7d5",chess::Move(true,ToBit("c7"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Ne7xd5","e7d5",chess::Move(true,ToBit("e7"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Nf6xd5","f6d5",chess::Move(true,ToBit("f6"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Nf4xd5","f4d5",chess::Move(true,ToBit("f4"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Knight,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				
				MoveEncodings("Bf3xd5","f3d5",chess::Move(true,ToBit("f3"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Bb3xd5","b3d5",chess::Move(true,ToBit("b3"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Bb7xd5","b7d5",chess::Move(true,ToBit("b7"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				MoveEncodings("Bf7xd5","f7d5",chess::Move(true,ToBit("f7"),ToBit("d5"), encode_flags(0,chess::BoardPiece::bp_Bishop,0)),"8/1BNRNB2/1N3N2/1R1q1R2/1N3N2/1BNRNB2/8/4k2K w - - 0 1"),
				
				MoveEncodings("Qf6xd4","f6d4",chess::Move(true,ToBit("f6"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"k6K/8/1Q1Q1Q2/8/1Q1r1Q2/8/1Q1Q1Q2/8 w - - 0 1"),
				MoveEncodings("Qf4xd4","f4d4",chess::Move(true,ToBit("f4"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"k6K/8/1Q1Q1Q2/8/1Q1r1Q2/8/1Q1Q1Q2/8 w - - 0 1"),
				MoveEncodings("Qf2xd4","f2d4",chess::Move(true,ToBit("f2"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"k6K/8/1Q1Q1Q2/8/1Q1r1Q2/8/1Q1Q1Q2/8 w - - 0 1"),
				MoveEncodings("Qd6xd4","d6d4",chess::Move(true,ToBit("d6"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"k6K/8/1Q1Q1Q2/8/1Q1r1Q2/8/1Q1Q1Q2/8 w - - 0 1"),
				MoveEncodings("Qd2xd4","d2d4",chess::Move(true,ToBit("d2"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"k6K/8/1Q1Q1Q2/8/1Q1r1Q2/8/1Q1Q1Q2/8 w - - 0 1"),
				MoveEncodings("Qb6xd4","b6d4",chess::Move(true,ToBit("b6"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"k6K/8/1Q1Q1Q2/8/1Q1r1Q2/8/1Q1Q1Q2/8 w - - 0 1"),
				MoveEncodings("Qb4xd4","b4d4",chess::Move(true,ToBit("b4"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"k6K/8/1Q1Q1Q2/8/1Q1r1Q2/8/1Q1Q1Q2/8 w - - 0 1"),
				MoveEncodings("Qb2xd4","b2d4",chess::Move(true,ToBit("b2"),ToBit("d4"), encode_flags(0,chess::BoardPiece::bp_Queen,0)),"k6K/8/1Q1Q1Q2/8/1Q1r1Q2/8/1Q1Q1Q2/8 w - - 0 1"),			
			},

			//promotion
			{ // 10

			},

			// check
			{ // 11

			},

			// mate
			{ // 12

			},
			
			// enpassant
			{ // 13
				MoveEncodings("cxb3", "c4b3", chess::Move(false, ToBit("c4"), ToBit("b3"), encode_flags(2, chess::BoardPiece::bp_Pawn, 0)), "6k1/6p1/8/5P1P/pPp5/8/8/6K1 b - b3 0 1"),
				MoveEncodings("axb3", "a4b3", chess::Move(false, ToBit("a4"), ToBit("b3"), encode_flags(2, chess::BoardPiece::bp_Pawn, 0)), "6k1/6p1/8/5P1P/pPp5/8/8/6K1 b - b3 0 1"),
				MoveEncodings("fxg6", "f5g6", chess::Move(true, ToBit("f5"), ToBit("g6"), encode_flags(2, chess::BoardPiece::bp_Pawn, 0)), "6k1/8/8/5PpP/pPp5/8/8/6K1 w - g6 0 2"),
				MoveEncodings("hxg6", "h5g6", chess::Move(true, ToBit("h5"), ToBit("g6"), encode_flags(2, chess::BoardPiece::bp_Pawn, 0)), "6k1/8/8/5PpP/pPp5/8/8/6K1 w - g6 0 2"),
			}
	};
    INSTANTIATE_TEST_SUITE_P(ClassicBitBoard, MoveTest, testing::ValuesIn(testMoves));
	
	#pragma endregion

	#pragma region Hash
	TEST(HashTest, generateHash) {
		chess::ClassicBitBoard board;
		uint64_t hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x463b96181691fc9c), hash) << "Fail starting position";
		
		std::string lan = std::string("e2e4");
		board.makeMove(board.LAN2Move(lan));
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x823c9b50fd114196), hash) << "Fail starting position e2e4";

		lan = std::string("d7d5");
		board.makeMove(board.LAN2Move(lan));
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x0756b94461c50fb0), hash) << "Fail starting position e2e4 d7d5";

		lan = std::string("e4e5");
		board.makeMove(board.LAN2Move(lan));
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x662fafb965db29d4), hash) << "Fail starting position e2e4 d7d5 e4e5";

		lan = std::string("f7f5");
		board.makeMove(board.LAN2Move(lan));
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x22a48b5a8e47ff78), hash) << "Fail starting position e2e4 d7d5 e4e5 f7f5";

		lan = std::string("e1e2");
		board.makeMove(board.LAN2Move(lan));
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x652a607ca3f242c1), hash) << "Fail starting position e2e4 d7d5 e4e5 f7f5 e1e2";

		lan = std::string("e8f7");
		board.makeMove(board.LAN2Move(lan));
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x00fdd303c946bdd9), hash) << "Fail starting position e2e4 d7d5 e4e5 f7f5 e1e2 e8f7";

		board.reset();
		std::vector<std::string> moves{ "a2a4","b7b5","h2h4" ,"b5b4","c2c4" };
		for (auto m : moves) {
			board.makeMove(board.LAN2Move(m));
		}		
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x3c8123ea7b067637), hash) << "Fail starting position a2a4 b7b5 h2h4 b5b4 c2c4";

		board.reset();
		moves = { "a2a4","b7b5","h2h4" ,"b5b4","c2c4" ,"b4c3", "a1a3"};
		for (auto m : moves) {
			board.makeMove(board.LAN2Move(m));
		}
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x5c3f9b829b279560), hash) << "Fail starting position a2a4 b7b5 h2h4 b5b4 c2c4 b4c3 a1a3";

		board.reset();
		moves = { "e2e4","b7b5","d2d4" ,"b5b4","a2a4"}; //EP on side of board;
		for (auto m : moves) {
			board.makeMove(board.LAN2Move(m));
		}
		hash = chess::ClassicBitBoard::HashUtil::createHash(board);
		ASSERT_EQ(U64(0x5dc5e204d03d3340), hash) << "Fail starting position e2e4 b7b5 d2d4 b5b4 a2a4";
	}
	#pragma endregion


	#pragma region SearchAgents
	void logOutput(chess::SearchAgents::Minimax agent) {
		std::cout << agent.nodes << std::endl;
	}
	void logOutput(chess::SearchAgents::AlphaBeta agent) {
		std::cout << agent.nodes << std::endl;
	}
	void logOutput(chess::SearchAgents::IttAlphaBeta agent) {
		std::cout << agent.nodes << std::endl;
	}
	TEST(SearchAgentsTest,go) {
		chess::ClassicBitBoard board;
		chess::SearchAgents::Minimax searchAgentMinimax;
		chess::SearchAgents::AlphaBeta searchAgentAB;
		chess::SearchAgents::IttAlphaBeta searchAgentABitt;
		chess::Move bestMoves[3];
		chess::Move ponder;
		// Setup of test:
		// Regardless of the searcAgent they should return the same value.
		int depth = 1;
		board.reset();
		searchAgentMinimax.minimax<chess::BetterAgent, true>(board, depth, bestMoves[0]);
		searchAgentAB.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[1]);
		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2], ponder);
		searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);
		EXPECT_EQ(bestMoves[0], bestMoves[1]);
		EXPECT_EQ(bestMoves[0], bestMoves[2]);
		EXPECT_EQ(bestMoves[1], bestMoves[2]);

		std::cout << depth << ":AB best move: " << bestMoves[1] << std::endl;
		std::cout << "Minimax: " << searchAgentMinimax.nodes << std::endl;
		std::cout << "AB     : " << searchAgentAB.nodes << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;

		depth = 2;
		board.reset();
		searchAgentABitt.TTtable.clear();
		searchAgentMinimax.minimax<chess::BetterAgent, true>(board, depth, bestMoves[0]);
		searchAgentAB.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[1]);
		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);

		EXPECT_EQ(bestMoves[0], bestMoves[1]);
		EXPECT_EQ(bestMoves[0], bestMoves[2]);
		EXPECT_EQ(bestMoves[1], bestMoves[2]);

		std::cout << depth << ":AB best move: " << bestMoves[1] << std::endl;
		std::cout << "Minimax: " << searchAgentMinimax.nodes << std::endl;
		std::cout << "AB     : " << searchAgentAB.nodes << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;

		depth = 3;
		board.reset();
		searchAgentABitt.TTtable.clear();
		searchAgentMinimax.minimax<chess::BetterAgent, true>(board, depth, bestMoves[0]);
		searchAgentAB.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[1]);
		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);

		EXPECT_EQ(bestMoves[0], bestMoves[1]);
		EXPECT_EQ(bestMoves[0], bestMoves[2]);
		EXPECT_EQ(bestMoves[1], bestMoves[2]);

		std::cout << depth << ":AB best move: " << bestMoves[1] << std::endl;
		std::cout << "Minimax: " << searchAgentMinimax.nodes << std::endl;
		std::cout << "AB     : " << searchAgentAB.nodes << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;

		depth = 5;
		board.reset();
		searchAgentABitt.TTtable.clear();
		searchAgentMinimax.minimax<chess::BetterAgent, true>(board, depth, bestMoves[0]);
		searchAgentAB.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[1]);
		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);

		EXPECT_EQ(bestMoves[0], bestMoves[1]);
		EXPECT_EQ(bestMoves[0], bestMoves[2]);
		EXPECT_EQ(bestMoves[1], bestMoves[2]);

		std::cout << depth << ":AB best move: " << bestMoves[1] << std::endl;

		std::cout << "Minimax: " << searchAgentMinimax.nodes << std::endl;
		std::cout << "AB     : " << searchAgentAB.nodes << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;
		
		depth = 6;
		board.reset();
		searchAgentABitt.TTtable.clear();
		//searchAgentMinimax.minimax<chess::BetterAgent, true>(board, depth, bestMoves[0]);
		searchAgentAB.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[1]);
		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);
		//EXPECT_EQ(bestMoves[0], bestMoves[1]);
		//EXPECT_EQ(bestMoves[0], bestMoves[2]);
		EXPECT_EQ(bestMoves[1], bestMoves[2]);
		
		std::cout << depth << ":AB best move: " << bestMoves[1] << std::endl;

		//std::cout << "Minimax: " << searchAgentMinimax.nodes << std::endl;
		std::cout << "AB     : " << searchAgentAB.nodes << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;

		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);
		std::cout << "ABitt not reset, repeated search:" << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;

		depth = 7;
		board.reset();
		searchAgentABitt.TTtable.clear();
		//searchAgentMinimax.minimax<chess::BetterAgent, true>(board, depth, bestMoves[0]);
		searchAgentAB.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[1]);
		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2], ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);

		//EXPECT_EQ(bestMoves[0], bestMoves[1]);
		//EXPECT_EQ(bestMoves[0], bestMoves[2]);
		EXPECT_EQ(bestMoves[1], bestMoves[2]);

		std::cout << depth << ":AB best move: " << bestMoves[1] << std::endl;

		//std::cout << "Minimax: " << searchAgentMinimax.nodes << std::endl;
		std::cout << "AB     : " << searchAgentAB.nodes << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;

		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);
		std::cout << "ABitt not reset, repeated search:" << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;


		depth = 8;
		board.reset();
		searchAgentABitt.TTtable.clear();
		//searchAgentMinimax.minimax<chess::BetterAgent, true>(board, depth, bestMoves[0]);
		searchAgentAB.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[1]);
		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);

		//EXPECT_EQ(bestMoves[0], bestMoves[1]);
		//EXPECT_EQ(bestMoves[0], bestMoves[2]);
		EXPECT_EQ(bestMoves[1], bestMoves[2]);

		std::cout << depth << ":AB best move: " << bestMoves[1] << std::endl;

		//std::cout << "Minimax: " << searchAgentMinimax.nodes << std::endl;
		std::cout << "AB     : " << searchAgentAB.nodes << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;

		////searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
        searchAgentABitt.limits.depth=depth;
        searchAgentABitt.search<chess::BetterAgent>(board,bestMoves[2],ponder);
		std::cout << "ABitt not reset, repeated search:" << std::endl;
		std::cout << "ABitt  : " << searchAgentABitt.nodes << std::endl;
		std::cout << " hits  : " << searchAgentABitt.tableHits << std::endl;

		//Note: all previous results yield worse values for ABitt
		
		//EXPECT_NO_THROW(logOutput(searchAgentMinimax));
		//EXPECT_NO_THROW(logOutput(searchAgentAB));
		//EXPECT_NO_THROW(logOutput(searchAgentABitt));
		//FAIL();
	}
	#pragma endregion
}