#include <iostream>
#include "PerfT.h"
#include <algorithm>

UCI::PerfT::PerfT() {
	this->board = chess::ClassicBitBoard();
	this->depth = -1;
}

UCI::PerfT::PerfT(std::string fen) {
	this->board = chess::ClassicBitBoard(fen);
	this->depth = -1;
}

UCI::PerfT::PerfT(std::string fen, int depth) {
	this->board = chess::ClassicBitBoard(fen);
	this->depth = depth;
}

UCI::PerfT::PerfT(chess::ClassicBitBoard& board)
{
	this->board = board;
	this->depth = -1;
}

void UCI::PerfT::start()
{
	if (depth == -1) { return; }
	moves.resize(depth);
	moves[depth - 1].clear();
	//std::vector<chess::Move> moves;
	board.generate_moves(moves[depth-1]);

	for (int i = 0; i < moves[depth - 1].size(); i++) {
		this->results.push_back(std::pair("", 0));
		//board.makeMove(moves[i]);
		board.makeMove(moves[depth-1][i]);
		perfTDriver(depth - 1);
		board.undoMove();

		//print results
		if (printAll) std::cout << this->results.back().first << " " << this->results.back().second << std::endl;
	}
}

void UCI::PerfT::start(int depth)
{
	this->depth = depth;
	if (depth == 0) {
		return;
	}
	//std::vector<chess::Move> moves;
	moves.resize(depth);
	moves[depth - 1].clear();
	board.generate_moves(moves[depth-1]);

	for (int i = 0; i < moves[depth - 1].size(); i++) {
		this->results.push_back(std::pair(moves[depth-1][i].toLAN(), 0));
		//board.makeMove(moves[i]);
		board.makeMove(moves[depth - 1][i]);
		perfTDriver(depth - 1);
		board.undoMove();

		//print results
		if (printAll) std::cout << this->results.back().first << " " << this->results.back().second << std::endl;
		this->totalNodes += results.back().second;
	}

	std::cout << "Totalnodes: " << this->totalNodes << std::endl;
}

void UCI::PerfT::start(int depth, std::string move) {
	this->depth = depth;
	//std::vector<chess::Move> moves;
	moves.resize(depth);
	moves[depth - 1].clear();
	board.generate_moves(moves[depth-1]);

	for (int i = 0; i < moves[depth - 1].size(); i++) {
		this->results.push_back(std::pair(moves[depth - 1][i].toLAN(), 0));
		if (moves[depth - 1][i].toLAN() == move) {
			//board.makeMove(moves[i]);
			board.makeMove(moves[depth - 1][i]);
			perfTDriver(depth - 1);
			board.undoMove();
			//print results
			if (printAll) std::cout << this->results.back().first << " " << this->results.back().second << std::endl;
			this->totalNodes += results.back().second;
		}		
	}

	std::cout << "Totalnodes: " << this->totalNodes << std::endl;
}
void UCI::PerfT::reset()
{
	results.clear();
	totalNodes = 0;
	//note: board should be back in original state after a perf test;
}

void UCI::PerfT::reset(std::string fen)
{
	results.clear();
	totalNodes = 0;
	this->board = chess::ClassicBitBoard(fen);
}

long long unsigned UCI::PerfT::getNodes()
{
	return totalNodes;
}

void UCI::PerfT::setPrint(bool print)
{
	this->printAll = print;
}

void UCI::PerfT::perfTDriver(int depth)
{
	if (depth <= 0) {
		results.back().second++;
	}
	else if (depth == 1) { //faster! because a move doesn't has to be made to count them!
		//std::vector<chess::Move> moves;
		moves[depth - 1].clear();
		board.generate_moves(moves[depth - 1]);
		results.back().second+=moves[depth - 1].size();
	}
	else {
		//std::vector<chess::Move> moves;
		moves[depth - 1].clear();
		board.generate_moves(moves[depth - 1]);
		for (int i = 0; i < moves[depth - 1].size(); i++) {
			board.makeMove(moves[depth - 1][i]);
			perfTDriver(depth - 1);
			board.undoMove();
		}
	}
	return;
}
