#pragma once
#include <chrono>
namespace UCI {

	class Limits
	{
	private:
		long long maxSearchTime=-1;
		long long lastItt=-1;
		long long elapsed = 0;
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
		long long getElapsed();
		long long elapsedSinceStart();
		long long elapsedMicrosecondsSinceStart();
		void startSearch(bool side);
		void nextItt();
		bool exeeded(int depth);
		bool exeededTime();
	};

}

