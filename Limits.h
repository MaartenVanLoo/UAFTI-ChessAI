#pragma once
#include <chrono>
namespace UCI {

	class Limits
	{
	private:
		long maxSearchTime=-1;
		long lastItt=-1;
		unsigned long elapsed = 0;
		std::chrono::steady_clock::time_point startItt = std::chrono::steady_clock::now();
	public:
		int depth = -1;
		std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
		long btime=-1;
		long wtime=-1;
		long winc=-1;
		long binc=-1;
		long movetime=-1;

		Limits();
		void setDefault();
		unsigned long getElapsed();
		unsigned long elapsedSinceStart();
		unsigned long elapsedMicrosecondsSinceStart();
		void startSearch(bool side);
		void nextItt();
		bool exeeded(int depth);
		bool exeededTime();
	};

}

