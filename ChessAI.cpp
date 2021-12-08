// ChessAI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "src/UCI/PerfT.h"
#include "src/Agents/SearchAgents.h"
#include "src/Agents/EvalAgents.h"
#include <chrono>
#include <windows.h>
#include "src/UCI/UCI.h"

#undef min
#undef max

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "src/Agents/TranspositionTable.h"
#include <unordered_set>
#include "src/chess/Polyglot.h"

static std::vector<std::string> GetElements(std::string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> splittedStrings;
	while (std::getline(ss, item, delimeter))
	{
		splittedStrings.push_back(item);
	}
	return splittedStrings;
}
void info();
void Mate2Bishops();
void Mate2Rooks();
void MateBN();
void AISelfplay(std::ofstream& log, std::string fen = "", int depth = 7);
void pgnTest(bool legalCheck, std::unordered_set<std::string>& fenSet);
void pgnBench();
void trainingSet();
void testSearchAgents();
void polyglot();
int main()
{
	/*chess::ClassicBitBoard board;
	std::vector<chess::Move> moves;
	std::string test_SAN = "1.e4 c5 2.Nc3 e6 3.Nf3 a6 4.d4 cxd4 5.Nxd4 Qc7 6.Be2 d6 7.O-O b5 8.Re1 Bb7 9.Bh5 g6 10.Bg4 b4 11.Na4 Nf6 12.Bxe6 fxe6 13.Nxe6 Qd7 14.Nxf8 Rxf8 15.Nb6 Qc6 16.Nxa8 Bxa8 17.Bf4 Rf7 18.Qxd6 Qxd6 19.Bxd6 Nc6 20.Rad1 Rd7 21.f3 Bb7 22.Bc5 Rxd1 23.Rxd1 Nd7 24.Bd6 a5 25.Kf2 Nde5 26.b3 h5 27.Bxe5 Nxe5 28.Rd6 Ke7 29.Rb6 Bc8 30.Ke3 Bd7 31.Kd4 Nc6+ 32.Kd5 Nd8 33.Rxg6 Ne6 34.Ke5 h4 35. f4 Nd8 36.f5 Be8 37.Ra6 a4 38.bxa4 Bf7 39.Ra7+ Ke8 40.Kf4 Bxa2 41.Re7+ Kf8 42.Rc7 Bb1 43.a5 b3 44.cxb3 Bxe4 45.a6";
	chess::ClassicBitBoard::pgnParser(test_SAN, moves);
	for (auto& m : moves) {
		std::cout << board << std::endl;
		std::cout << board.getFen() << std::endl;
		system("pause");
		board.makeMove(m);
		
	}
	std::cout << board << std::endl;
	std::cout << board.getFen() << std::endl;
	system("pause");
	*/
	//info();
	//system("pause");
	//polyglot();
	//testSearchAgents();	
	//trainingSet();
	//system("pause");
	//return 0;
	//pgnTest(true);
	//for (int i = 0; i < 100; i++) {
	//	pgnBench();
	//}	
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.dwFontSize.X = 0;                   // Width of each character in the font
	cfi.dwFontSize.Y = 19;                  // Height
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, r.top, 900, 640, TRUE); // 800 width, 100 height

	//info();
	//chess::TranspositionTable tt(4096);
	UCI::UCI uciInterface;
	uciInterface.start();
	return 0;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::ofstream gameLog;
    gameLog.open("LogGame.txt");
    if (gameLog.fail()) {
    	return -1;
    }

	//AISelfplay(gameLog,"8/1p4q1/1B1p4/P2P4/5k2/8/5K2 b - - 0 0",7);
	//AISelfplay(gameLog,"",8);
	//Mate2Bishops();
	//Mate2Rooks();
	//MateBN();
	system("pause");
	return 0;




	std::string dbg = "rnbqkbnr/ppppppp1/8/7p/8/3P4/PPPKPPPP/RNBQ1BNR w kq - 0 3"; //after moves   d2d3 h7h6 e1d2 h6h5
	std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string kiwi = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"; //kiwi
    std::string midgame = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    std::string endgame = "5nk1/pp3pp1/2p4p/q7/2PPB2P/P5P1/1P5K/3Q4 w - - 1 28";

    UCI::PerfT perft(startpos);
    std::cout << "Startpos: ";
	perft.setPrint(false);
	for (int i = 1; i < 10; i++) {
		std::cout << "\nPERFT depth: " << i << std::endl;
		begin = std::chrono::steady_clock::now(); 
		perft.start(i);
		
		end = std::chrono::steady_clock::now();
		long long unsigned elapsedseconds = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
		std::cout << "Time difference = " <<elapsedseconds << "[ms]" << std::endl;
		std::cout << "Nodes/s: " << (elapsedseconds==0?perft.getNodes():uint64_t(perft.getNodes() / (elapsedseconds+0.000000001)))<< "kN/s" << std::endl;
		perft.reset();
	}
	//return 0; // optional return value
    //std::cout << "Hello World!\n";
}

void info() {
	std::cout << "Printing Info:\n"
		<< "Size of bitboard:" << sizeof(chess::ClassicBitBoard) << "\n"
		<< "Size of move    :" << sizeof(chess::Move) << "\n"
		<< "Size of History :" << sizeof(chess::ClassicBitBoard::History) << "\n"
		<< "Size of TTentry :" << sizeof(chess::TTentry) << "\n";
	std::cout << std::endl;
}
void Mate2Bishops() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::ofstream gameLog;
	gameLog.open("Mate2Bishops.txt");
	if (!gameLog.fail()) {
		AISelfplay(gameLog,"8/8/2k5/8/8/1B6/2KB4/8 w - - 0 10", 8);
	}
	
}
void Mate2Rooks() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::ofstream gameLog;
	gameLog.open("Mate2Rooks.txt");
	if (!gameLog.fail()) {
		AISelfplay(gameLog, "8/8/2k5/8/8/1R6/3R4/K7 w - - 0 10", 8);
	}
}

void MateBN()
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::ofstream gameLog;
	gameLog.open("MateBN.txt");
	if (!gameLog.fail()) {
		AISelfplay(gameLog, "8/8/8/3k4/8/8/1B2K1N1/8 w - - 0 10",12);
	}
}

void AISelfplay(std::ofstream& log,std::string fen, int depth) {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	chess::ClassicBitBoard brd;
	chess::SearchAgents::IttAlphaBeta searchAgent;
	chess::SearchAgents::PVS searchAgent2;

	if (fen != "") {
		brd = chess::ClassicBitBoard(fen);
	}
	
	const int depth_minmax = std::min(depth, 6);
	chess::Move nextMove;
	chess::Move ponderMove;
	std::vector<chess::Move> moves;
	std::cout << brd.toASCII();
	do {
		int value;
		long long unsigned elapsedseconds;
		begin = std::chrono::steady_clock::now();
		/*
		//minimax
		if (brd.side) {
			value = chess::SearchAgents::Minimax::minimax<chess::SimpleAgent, true>(brd, depth_minmax, nextMove);
		}
		else {
			value = chess::SearchAgents::Minimax::minimax<chess::SimpleAgent, false>(brd, depth_minmax, nextMove);
		}
		end = std::chrono::steady_clock::now();
		elapsedseconds = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
		std::cout << "Minimax:" <<std::endl;
		std::cout << "Best move eval:" << value << std::endl;
		std::cout << "Best move:" << brd.MovetoSAN(nextMove) << std::endl;
		std::cout << "From: " << SquareOf(nextMove.from) << "\tTo: " << SquareOf(nextMove.to) << std::endl;
		std::cout << "Nodes evaluated: " << uint64_t(chess::SearchAgents::Minimax::nodes) << std::endl;
		std::cout << "Time difference = " << elapsedseconds / 1000 << " [s]" << std::endl;
		std::cout << "Nodes/s: " << (elapsedseconds == 0 ? chess::SearchAgents::Minimax::nodes : uint64_t(chess::SearchAgents::Minimax::nodes / (elapsedseconds + 0.000000001))) << " kN/s\n" << std::endl;
		*/
		//alpha beta
		begin = std::chrono::steady_clock::now();
        value = searchAgent2.search<chess::BetterAgent>(brd, depth, nextMove, ponderMove);
        value = searchAgent.search<chess::BetterAgent>(brd, depth, nextMove, ponderMove);

		end = std::chrono::steady_clock::now();
		elapsedseconds = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
		std::cout << "Alpha beta:" << std::endl;
		std::cout << "Best move eval:" << value << std::endl;
		std::cout << "Best move     :" << brd.Move2SAN(nextMove) << std::endl;
		std::cout << "From: " << SquareOf(nextMove.from) << " \tTo: " << SquareOf(nextMove.to) << std::endl;
		std::cout << "Nodes evaluated  : " << uint64_t(searchAgent.nodes) << std::endl;
		std::cout << "Mates found      : " << uint64_t(searchAgent.mates) << std::endl;
		std::cout << "Draws found      : " << uint64_t(searchAgent.draws) << std::endl;
		std::cout << "Repetitions found: " << uint64_t(searchAgent.threefold) << std::endl;
		std::cout << "Time difference  : " << elapsedseconds / 1000 << " [s]" << std::endl;
		std::cout << "Nodes/s          : " << (elapsedseconds == 0 ? searchAgent.nodes : uint64_t(searchAgent.nodes / (elapsedseconds + 0.000000001))) << " kN/s\n" << std::endl;
		std::cout << "Halfmoves        : " << brd.halfmoves << std::endl;
		if (!log.fail()) {
			log << "Alpha beta:" << std::endl;
			log << "Best move eval:" << value << std::endl;
			log << "Best move     :" << brd.Move2SAN(nextMove) << std::endl;
			log << "From: " << SquareOf(nextMove.from) << " \tTo: " << SquareOf(nextMove.to) << std::endl;
			log << "Nodes evaluated  : " << uint64_t(searchAgent.nodes) << std::endl;
			log << "Mates found      : " << uint64_t(searchAgent.mates) << std::endl;
			log << "Draws found      : " << uint64_t(searchAgent.draws) << std::endl;
			log << "Repetitions found: " << uint64_t(searchAgent.threefold) << std::endl;
			log << "Time difference  : " << elapsedseconds / 1000 << " [s]" << std::endl;
			log << "Nodes/s          : " << (elapsedseconds == 0 ? searchAgent.nodes : uint64_t(searchAgent.nodes / (elapsedseconds + 0.000000001))) << " kN/s\n" << std::endl;
			log << "Halfmoves        : " << brd.halfmoves << std::endl;
		}
		//make move
		brd.makeMove(nextMove);
		std::cout << brd << std::endl;
		if (!log.fail()) { log << brd << std::endl; }
		moves.clear();
		brd.generate_moves(moves);
	} while (!brd.isThreeFold() && moves.size() > 0 && brd.halfmoves < 100);
	bool IsCheck = brd.side ? brd.isCheck<true>() : brd.isCheck < false>();
	if (brd.isThreeFold() || brd.halfmoves >= 100) {
		std::cout << "Draw" << std::endl;
		log << "Draw" << std::endl;
	}
	else if (moves.size() == 0 && IsCheck) {
		if (brd.side) {
			std::cout << "Black win" << std::endl;
			log << "Black win" << std::endl;
		}
		else {
			std::cout << "White win" << std::endl;
			log << "White win" << std::endl;
		}
	}
}

void pgnTest(bool legalCheck, std::unordered_set<std::string>& fenSet){
	std::string pgn = "1.d4 Nf6 2.Nf3 e6 3.c4 b6 4.a3 Bb7 5.Nc3 d5 6.Bg5 Be7 7.Qa4+ c6 8.Bxf6 Bxf6 9.cxd5 exd5 10.g3 O-O 11.Bg2 c5 12.Rd1 c4 13.O-O a6 14.Ne5 b5 15.Qc2 Qd6 16.e4 Bxe5 17.dxe5 Qxe5 18.f4 Qd6 19.Nxd5 Nd7 20.Qf2 Rad8 21.Rd2 Bxd5 22.Rxd5 Qc7 23.Rfd1 Nb6 24.R5d4 Na4 25.e5 Rxd4 26.Qxd4 c3 27.bxc3 Nxc3 28.Re1 Rd8 29.Qe3 Nd1 30.Qe2 Qc5+ 31.Kf1 Nc3 32.Qe3 Qc4+ 33.Kg1 a5 34.f5 b4 35.e6 fxe6 36.fxe6 Re8 37.e7 Qc8 38.axb4 axb4 39.Qd4 b3 40.Qb4 Kh8 41.Rf1 h6 42.Be4 Ne2+ 43.Kg2 g5 44.Qd6";
	std::vector<chess::Move> moves;
	chess::ClassicBitBoard::pgnParser(pgn, moves);
	std::vector<std::string> fens;
	fens.reserve(moves.size());
	chess::ClassicBitBoard board;
	for (auto const &m : moves) {
		//fens.push_back(board.getFen());
		fenSet.insert(board.getFen());
		board.makeMove(m);
	}

	std::vector<chess::Move> generatedMoves;
	//Check if legal:
	if (legalCheck) {
		board.reset();
		std::cout << board.getFen() << std::endl;
		std::cout << chess::ClassicBitBoard::startpos << std::endl;
		if (board.getFen() != chess::ClassicBitBoard::startpos) {
			std::cout << "ERROR" << std::endl;
		}
		for (auto& m : moves) {
			board.generate_moves(generatedMoves);
			for (int i = 0; i < generatedMoves.size(); i++) {
				if (m == generatedMoves[i]) break;
				if (i == generatedMoves.size() - 1) {
					//last move was not equal => error:
					std::cout << "ERROR detexted in pgn parser for move " << m << "at index " << i << "." << std::endl;
				}
			}
			board.makeMove(m);
		}
	}
}

void pgnBench() {
	std::unordered_set<std::string> fenSet;
	fenSet.reserve(1 << 10);
	long long unsigned n = 2500;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
	for (int i = 0; i < n; i++) {
		pgnTest(false, fenSet);
		//std::cout << ":" << fenSet.size() << std::endl;
	}
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	for (int i = 0; i < 77; i++) {
		std::cout << *fenSet.begin() << std::endl;
		fenSet.erase(fenSet.begin());
	}
	long long unsigned elapsedseconds = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
	std::cout << "Elapsed time: " << elapsedseconds << " ms" << std::endl;
	std::cout << "Pgn games/s:  " << n*1000 / elapsedseconds << " games/s" << std::endl;

}

void trainingSet() {
	std::unordered_set<std::string> fenSet;
	fenSet.reserve(150000000);
	std::vector<chess::Move> moves; 
	chess::ClassicBitBoard board;

	std::ifstream pgn;
	pgn.open("ChessData/PGN/OTB-HQ_Cleaned.pgn");
	if (pgn.fail()) {
		std::cout << "Error";
		return;
	}
	std::string san;
	long count = 0;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	while (std::getline(pgn, san)) {
		count++;
		//std::cout << san << std::endl;
		
		if (!chess::ClassicBitBoard::pgnParser(san, moves)) {
			std::cout << "Parse error at " << count << " " << san << std::endl;
			continue;
		}
		board.reset();
		for (auto &m : moves) {
			fenSet.insert(board.getFen());
			/*if (board.Occ == 616149285066679826) {
				std::cout << count << " : " << board.getFen() << std::endl;
				std::cout << "SAN: " << san << std::endl;
				system("pause");
			}*/
			board.makeMove(m);
		}
		if (count % 2683 == 0) {
			end = std::chrono::steady_clock::now();
			long long unsigned elapsedseconds = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
			std::cout << "Count: " <<std::setw(7) << count << "\tElapsed time:" << std::setw(7) << elapsedseconds << "\tGames/s: " << std::setw(5) << (count * 1000ull) / elapsedseconds << "\tAvg positions/game: " << fenSet.size()/count << "\tFound positions " << fenSet.size() << std::endl;
		}
	}

	//after all is done, write to files of 4194304 (1<<22) fens/file
	std::stringstream ss;
	const std::string folder = "ChessData/FEN/";
	const std::string baseFileName = "FenBatch";
	const std::string extension = ".txt";
	std::string stringIndex;
	int index = 0;
	int batchCount = 0;
	
	while (fenSet.size() > 0) {
		ss.str(std::string());
		ss << std::setw(2) << std::setfill('0') << index;
		stringIndex = ss.str();
		std::string fileName = folder + baseFileName + stringIndex + extension;
		while (std::filesystem::exists(fileName)) {
			index++;
			std::cout << fileName << " already exists" << std::endl;
			fileName = folder + baseFileName + std::to_string(index) + extension;			
		}
		batchCount++;
		std::cout << "Witing file " << fileName << "\t Elements left: " << fenSet.size() << std::endl;
		long elementsWritten = 0;
		std::ofstream file(fileName);
		if (file.fail()) {
			std::cout << "failed to open";
			return;
		}
		while (elementsWritten < (1 << 22) && fenSet.size() > 0) {
			file << *fenSet.begin() <<"\n";
			fenSet.erase(fenSet.begin());
			elementsWritten++;
		}
		index++;
	}
	std::cout << "Done, " << batchCount << " batches written" << std::endl;

}

void testSearchAgents() {
	chess::ClassicBitBoard board;
	chess::SearchAgents::Minimax searchAgentMinimax;
	chess::SearchAgents::AlphaBeta searchAgentAB;
	chess::SearchAgents::IttAlphaBeta searchAgentABitt;
	chess::Move bestMoves[3];
	chess::Move ponder;
	bool activeAgents[][3] = {
		{true, true,true}, //depth 1
		{true, true,true}, //depth 2
		{true, true,true}, //depth 3
		{true, true,true}, //depth 4
		{false, false,true}, //depth 5
		{false, false,true}, //depth 6
		{false, false,true}, //depth 7
		{false, false,true}, //depth 8
		//{false, false,true}, //depth 9
	};
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end0 = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end1 = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
	long long unsigned elapsedseconds0 = std::chrono::duration_cast<std::chrono::milliseconds> (end0 - begin).count();
	long long unsigned elapsedseconds1 = std::chrono::duration_cast<std::chrono::milliseconds> (end0 - begin).count();
	long long unsigned elapsedseconds2 = std::chrono::duration_cast<std::chrono::milliseconds> (end0 - begin).count();
	// Setup of test:
	// Regardless of the searcAgent they should return the same value.
	int depth = 0;
	for (auto active: activeAgents){
		depth++; if (depth == 7) break;
		board.reset();
		searchAgentABitt.TTtable.clear();
		begin = std::chrono::steady_clock::now();
		if (active[0]) searchAgentMinimax.search<chess::BetterAgent>(board, depth, bestMoves[0],ponder);
		end0 = std::chrono::steady_clock::now();
		if (active[1]) searchAgentAB.search<chess::BetterAgent>(board, depth, bestMoves[1],ponder);
		end1 = std::chrono::steady_clock::now();
		////if (active[2]) searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
		end2 = std::chrono::steady_clock::now();
		elapsedseconds0 = std::chrono::duration_cast<std::chrono::milliseconds> (end0 - begin).count();
		elapsedseconds1 = std::chrono::duration_cast<std::chrono::milliseconds> (end1 - end0).count();
		elapsedseconds2 = std::chrono::duration_cast<std::chrono::milliseconds> (end2 - end1).count();

		if (bestMoves[0] != bestMoves[1] && active[0] && active[1]) std::cerr << depth << ":move[0] != move[1]" << std::endl;
		if (bestMoves[0] != bestMoves[2] && active[0] && active[2]) std::cerr << depth << ":move[0] != move[2]" << std::endl;
		if (bestMoves[1] != bestMoves[2] && active[1] && active[2]) std::cerr << depth << ":move[1] != move[2]" << std::endl;

		if (active[1]) std::cout << depth << ":AB    best move: " << bestMoves[1] << std::endl;
		if (active[2]) std::cout << depth << ":ABitt best move: " << bestMoves[2] << std::endl;

		if (active[0]) std::cout << "Minimax: " << searchAgentMinimax.nodes << "\ttime:" << elapsedseconds0 << std::endl;
		if (active[1]) std::cout << "AB       : " << searchAgentAB.nodes << "\ttime:" << elapsedseconds1 << std::endl;
		if (active[2]) std::cout << "ABitt    : " << searchAgentABitt.nodes << "\ttime:" << elapsedseconds2 << std::endl;
		if (active[2]) std::cout << " hits    : " << searchAgentABitt.tableHits << std::endl;
		if (active[2]) std::cout << " collis  : " << searchAgentABitt.TTtable.getCollisions() << std::endl;
		if (active[2]) std::cout << " pvloss  : " << searchAgentABitt.TTtable.getpvloss() << std::endl;
		if (active[2]) std::cout << " entries : " << searchAgentABitt.TTtable.getEntries() << std::endl;
		if (active[2]) std::cout << " TTsize  : " << searchAgentABitt.TTtable.size() << std::endl;
		if (active[2]) std::cout << " threeF  : " << searchAgentABitt.threefold << std::endl;
		if (active[2]) std::cout << " twoF    : " << searchAgentABitt.twofold << std::endl;

		end1 = std::chrono::steady_clock::now();
		//if (active[2]) searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder);
		end2 = std::chrono::steady_clock::now();
		elapsedseconds2 = std::chrono::duration_cast<std::chrono::milliseconds> (end2 - end1).count();
		if (active[2]) std::cout << "ABitt not reset, repeated search:" << std::endl;
		if (active[2]) std::cout << "ABitt    : " << searchAgentABitt.nodes << "\ttime:" << elapsedseconds2 << std::endl;
		if (active[2]) std::cout << " hits    : " << searchAgentABitt.tableHits << std::endl;
		if (active[2]) std::cout << " collis  : " << searchAgentABitt.TTtable.getCollisions() << std::endl;
		if (active[2]) std::cout << " pvloss  : " << searchAgentABitt.TTtable.getpvloss() << std::endl;
		if (active[2]) std::cout << " entries : " << searchAgentABitt.TTtable.getEntries() << std::endl;
		if (active[2]) std::cout << " TTsize  : " << searchAgentABitt.TTtable.size() << std::endl;
		if (active[2]) std::cout << " threeF  : " << searchAgentABitt.threefold << std::endl;
		if (active[2]) std::cout << " twoF    : " << searchAgentABitt.twofold << std::endl;
	}


	std::cout << "Simulating game" << std::endl;
	//simulated 50 move (8 halfmoves) game:
	int n = 50 * 2;
	for (int i = 6; i < std::size(activeAgents); i++) {
		auto active = activeAgents[i];
		depth = i + 1;
		std::cout << "\n\nDEPTH " << depth+1  << std::endl;
		board.reset();
		searchAgentABitt.TTtable.clear();
		for (int i = 0; i < n; i++) {
			std::cout << "\nMOVE " << i << std::endl;

			begin = std::chrono::steady_clock::now();
            if (active[0]) searchAgentMinimax.search<chess::BetterAgent>(board, depth, bestMoves[0],ponder);
            end0 = std::chrono::steady_clock::now();
            if (active[1]) searchAgentAB.search<chess::BetterAgent>(board, depth, bestMoves[1],ponder);
            end1 = std::chrono::steady_clock::now();
			//if (active[2]) (board.side) ? searchAgentABitt.alphabeta<chess::BetterAgent, true>(board, depth, bestMoves[2],ponder): searchAgentABitt.alphabeta<chess::BetterAgent, false>(board, depth, bestMoves[2],ponder);
			end2 = std::chrono::steady_clock::now();
			elapsedseconds0 = std::chrono::duration_cast<std::chrono::milliseconds> (end0 - begin).count();
			elapsedseconds1 = std::chrono::duration_cast<std::chrono::milliseconds> (end1 - end0).count();
			elapsedseconds2 = std::chrono::duration_cast<std::chrono::milliseconds> (end2 - end1).count();

			if (bestMoves[0] != bestMoves[1] && active[0] && active[1]) std::cerr << depth << ":move[0] != move[1]" << std::endl;
			if (bestMoves[0] != bestMoves[2] && active[0] && active[2]) std::cerr << depth << ":move[0] != move[2]" << std::endl;
			if (bestMoves[1] != bestMoves[2] && active[1] && active[2]) std::cerr << depth << ":move[1] != move[2]" << std::endl;

			if (active[1]) std::cout << depth << ":AB    bestmove: " << bestMoves[1] << std::endl;
			if (active[2]) std::cout << depth << ":ABitt bestmove: " << bestMoves[2] << " ponder " << ponder << std::endl;

			if (active[0]) std::cout << "Minimax: " << searchAgentMinimax.nodes << "\ttime:" << elapsedseconds0 << std::endl;
			if (active[1]) std::cout << "AB       : " << searchAgentAB.nodes << "\ttime:" << elapsedseconds1 << std::endl;
			if (active[2]) std::cout << "ABitt    : " << searchAgentABitt.nodes << "\ttime:" << elapsedseconds2 << std::endl;
			if (active[2]) std::cout << " hits    : " << searchAgentABitt.tableHits << std::endl;
			if (active[2]) std::cout << " collis  : " << searchAgentABitt.TTtable.getCollisions() << std::endl;
			if (active[2]) std::cout << " pvloss  : " << searchAgentABitt.TTtable.getpvloss() << std::endl;
			if (active[2]) std::cout << " entries : " << searchAgentABitt.TTtable.getEntries() << std::endl;
			if (active[2]) std::cout << " TTsize  : " << searchAgentABitt.TTtable.size() << std::endl;
			if (active[2]) std::cout << " threeF  : " << searchAgentABitt.threefold << std::endl;
			if (active[2]) std::cout << " twoF    : " << searchAgentABitt.twofold << std::endl;
			board.makeMove(bestMoves[2]);
		}
	}
}

void polyglot() {
	std::string b1 = "gm2600.bin";
	std::string b2 = "Performance.bin";
	chess::Polyglot book1;
	book1.loadBook(b1);
	//book1.printBook();
	//system("pause");
	chess::Polyglot book2;
	book2.loadBook(b2);
	//book2.printBook();

	std::string move;
	std::unordered_map<std::string, int> count;
	for (int i = 0; i < 100000; i++) {
		book1.getMove(0x823c9b50fd114196, move);
		count[move] += 1;		
	}
	for (const auto& [key, value] : count) {
		std::cout << "Key:[" << key << "] Value:[" << value << "]\n";
	}
	std::cout << move;
	std::cout << b1 << " conains " << book1.size() << " elements " << std::endl;
	std::cout << b2 << " conains " << book2.size() << " elements " << std::endl;
	
}
