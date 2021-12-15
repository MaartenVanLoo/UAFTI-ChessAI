#pragma once
#include "ClassicBitBoard.h"
#include <filesystem>

namespace UCI {
	class PerfT
	{
		chess::ClassicBitBoard board;
		long long unsigned totalNodes = 0;
		int depth = -1;
		
		bool printAll = false;
		std::vector < std::pair<std::string, long long unsigned>> results;

		std::vector<std::vector<chess::Move>> moves;
		
	public:
		PerfT();
		PerfT(std::string fen);
		PerfT(std::string fen, int depth);
		PerfT(chess::ClassicBitBoard& board);

		void start();
		void start(int depth);
		void start(int depth, std::string move);
		void reset();
		void reset(std::string fen);
		long long unsigned getNodes();

		void setPrint(bool print);
	private:
		void perfTDriver(int depth);
	};
}

