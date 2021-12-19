#pragma once
#include "../chess/ClassicBitBoard.h"
#include "../Agents/SearchAgents.h"
#include "../Agents/EvalAgents.h"
#include "UCIOptions.h"
#include <fstream>
#include <chrono>
#include <thread>
#include "../chess/Polyglot.h"
#include <future>

namespace UCI {
	/// <summary>
	/// <para>UCI interface.</para>
	/// <para>Current supported commands:</para>
	/// <para>- uci</para>
	/// <para>- debug[on|off]</para>
	/// <para>- isready</para>
	/// <para>- stoption  name "id" [value x]</para>
	/// <para>- position[fen "fenstring" |startpos] moves "move1" ... "movei"</para>
	/// <para>- go</para>
	/// <para>- stop</para>
	/// <para>- ponderhit</para>
	/// <para>- quit</para>
	/// </summary>
	class UCI
	{
		chess::ClassicBitBoard board;
		//chess::SearchAgents::AlphaBeta searchAgent;
		//chess::SearchAgents::IttAlphaBeta searchAgent;
		//chess::SearchAgents::PVS searchAgent;
		chess::SearchAgents::PVSRazoring searchAgent;
		chess::BetterAgent evalAgent;
		UCIOptions options;
		chess::Move bestmove;
		chess::Move ponder;

		std::ofstream logFile;
		bool debug = true;

		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::string ID = "FTI UA 5-AI-Engine";
		
		//polyglot book;
		const std::string polyglotBook = "gm2600.bin";
		chess::Polyglot book;
		chess::PolyglotPonder bookPonder;
		//Helper threads;
		std::future<void> polyglotPonder;// = std::promise<void>().get_future();
		//std::thread polyglot_thread;
	public:
		
		UCI();
		
		//Main uci loop
		void start();

	private:
		void init();
		void go(std::istringstream& is);
		void newgame();
		void position(std::istringstream& is);

	};
}


