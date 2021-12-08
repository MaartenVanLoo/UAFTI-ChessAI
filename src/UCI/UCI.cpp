#include "UCI.h"
#include  <ios>
#include  <sstream>
#include <iostream>
#include "PerfT.h"
#include <time.h>


UCI::UCI::UCI()
{
	init();
	if (this->debug) {
		time_t now = time(0);
		tm* ltm = new tm();
		localtime_s(ltm,&now);
		std::string str = "";
		str += std::to_string(1900+ ltm->tm_year);
		str += std::to_string(ltm->tm_mday);
		str += std::to_string(ltm->tm_hour);
		str += std::to_string(ltm->tm_min);
		str += std::to_string(ltm->tm_sec);	
		std::string folder = "logs";

		std::filesystem::create_directory(folder);
		std::string filename = folder + "/GameLog" + str + ".txt";
		this->logFile.open(filename);
		if (this->logFile.fail()) {
			std::cout << "No logging";
		}
		delete ltm;
	}
}

void UCI::UCI::start()
{
	std::string token, cmd;

	do {
		if (!std::getline(std::cin, cmd)) {
			cmd = "quit";
		}
		std::istringstream is(cmd);
		token.clear();

		is >> std::skipws >> token; // skipws = skip leading white space
		if (debug) logFile << "Recieved: " << token;

		if (token == "quit" || token == "stop") {
			std::cout << "info: stop" << std::endl;
			if (debug) logFile  << "\nAnswer: "<<  "info: stop" << std::endl;
		}
		else if(token == "ponderhit") {
			std::cout << "info: " << token << std::endl;
			if (debug) logFile << "\nAnswer: " << "info: " << token << std::endl;

		}
		else if (token == "uci") {
			std::cout << "id name " 
				<< this->ID << "\n" 
				<< options.listOptions()
				<< "\nuciok" << std::endl;
			if (debug) logFile << "\nAnswer: " << "id name "
				<< this->ID << "\n" 
				<< options.listOptions()
				<< "\nuciok" << std::endl;
		}
		else if (token == "setoption") {
			options.setoption(is,logFile);
		}
		else if (token == "go") {
			go(is);
		}
		else if (token == "position") {
			position(is);
		}
		else if (token == "ucinewgame") {
			newgame();
		}
		else if (token == "isready") {
			std::cout << "readyok" << std::endl;
			if (debug) logFile << "\nAnswer: " << "readyok" << std::endl;
		}
		else if (token == "d") {
			std::cout << board << std::endl;
			if (debug) logFile << "\nAnswer: " << board << std::endl;
		}
		else if (token == "eval") {
			int eval = evalAgent.eval(board);
			std::cout << board << "\n"
				<< "Evaluation: " << (eval>0?"+":"") << eval << std::endl;
			if (debug) logFile << "\nAnswer: " << board << "\n"
				<< "Evaluation: " << (eval>0?"+":"") << eval << std::endl;
		}
		else {
			std::cout << "info Command not recognised: " << token << std::endl;
			if (debug) logFile << "\nAnswer: " << "info Command not recognised: " << token << std::endl;
		}

	} while (token != "quit");
}

void UCI::UCI::init()
{
	board = chess::ClassicBitBoard(chess::ClassicBitBoard::startpos);
	searchAgent = chess::SearchAgents::PVS();
	evalAgent = chess::BetterAgent();
	options = UCIOptions();	
	book = chess::Polyglot(this->polyglotBook);
	bookPonder = chess::PolyglotPonder(book);
	searchAgent.setBook(book);

	//already start ponder for startpos
	//this->polyglotPonder = std::async(std::launch::async, &chess::PolyglotPonder::computePonder, std::ref(bookPonder), board);
}

void UCI::UCI::go(std::istringstream& is)
{
	//std::cout << "INFO go"<< std::endl;
	searchAgent.logFile.str("");
	std::string token;
	bool perft = false;
	searchAgent.limits.setDefault();
	while (is >> token) {
		logFile << " " << token;
		if (token == "depth") { is >> searchAgent.limits.depth; if (debug) logFile << " " << searchAgent.limits.depth; }
		else if (token == "perft") { perft = true; is >> searchAgent.limits.depth; if (debug) logFile << " " << searchAgent.limits.depth; }
		//else if (token == "movetime") { is >> searchAgent.limitTime; if (debug) logFile << " " << searchAgent.limitTime; }
		else if (token == "movetime") { is >> searchAgent.limits.movetime; if (debug) logFile << " " << searchAgent.limits.movetime; }
		else if (token == "infinite") { searchAgent.limits.depth = 255; searchAgent.limits.movetime = 100; if (debug) logFile << "infinite: depth 255, movetime 100 000"; }
		else if (token == "wtime") { is >> searchAgent.limits.wtime; if (debug) logFile << " " << searchAgent.limits.wtime; }
		else if (token == "btime") { is >> searchAgent.limits.btime; if (debug) logFile << " " << searchAgent.limits.btime; }
		else if (token == "winc") { is >> searchAgent.limits.winc; if (debug) logFile << " " << searchAgent.limits.winc; }
		else if (token == "binc") { is >> searchAgent.limits.binc; if (debug) logFile << " " << searchAgent.limits.binc; }
		else std::cout << "info unknown token: " << token << std::endl;
	}

	if (debug) logFile << "\nCurrent pos:\n" << this->board << std::endl;
	if (perft) {
		PerfT perft(board);
		perft.setPrint(true);
		perft.start(searchAgent.limits.depth);
	}
	else {	
		int value;
		/*
		uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
		if (this->book.getMove(key, bestmove, board)) {
			//std::cout << "info: theory hit" << std::endl;
			this->ponder = chess::Move();
			//get eval to return this value
			board.makeMove(bestmove);
			value = this->board.side ? chess::BetterAgent::eval<true>(board) : chess::BetterAgent::eval<false>(board);
			board.undoMove();
		}
		// else => search tree
		else {			
			//std::cout << "info: theory miss" << std::endl;
			if (this->board.side)
				value = searchAgent.alphabeta<chess::BetterAgent, true>(this->board, depth, this->bestmove, this->ponder);
			else
				value = searchAgent.alphabeta<chess::BetterAgent, false>(this->board, depth, this->bestmove, this->ponder);
		}*/		
		
		/*std::cout << "info depth " << depth
			<< " nodes " << searchAgent.nodes
			<< " nps " << (long long unsigned)(searchAgent.nodes * 1e6) / elapsedmicroseconds
			<< " mates " << searchAgent.mates
			<< " time " << elapsedmilliseconds
			<< std::endl;*/
		value = searchAgent.search<chess::BetterAgent>(board, bestmove, ponder);
		std::cout << "bestmove " << bestmove.toLAN() << " ponder " << ponder.toLAN() << std::endl;
		if (debug) {
			logFile << searchAgent.logFile.str();
			
			/*logFile << "info depth " << depth
				<< " nodes " << searchAgent.nodes
				<< " nps " << (long long unsigned)(searchAgent.nodes * 1e6) / elapsedmicroseconds
				<< " mates " << searchAgent.mates
				<< " time " << elapsedmilliseconds
				<< std::endl;*/
			logFile << "\nAnswer: " << "bestmove " << bestmove.toLAN() << " ponder " << ponder.toLAN() << std::endl;
			logFile << "Alpha beta:" << std::endl;
			logFile << "Best move eval:" << value << std::endl;
			logFile << "Best move     :" << this->board.Move2SAN(bestmove) << std::endl;
			logFile << "Nodes evaluated  : " << uint64_t(searchAgent.nodes) << std::endl;
			logFile << "Mates found      : " << uint64_t(searchAgent.mates) << std::endl;
			logFile << "Draws found      : " << uint64_t(searchAgent.draws) << std::endl;
			logFile << "Repetitions found: " << uint64_t(searchAgent.threefold) << std::endl;
			//logFile << "Time difference  : " << elapsedmilliseconds / 1000 << " [s]" << std::endl;
			//logFile << "Nodes/s          : " << (elapsedmilliseconds == 0 ? searchAgent.nodes : uint64_t(searchAgent.nodes / (elapsedmilliseconds + 0.000000001))) << " kN/s\n" << std::endl;
			logFile << "Halfmoves        : " << this->board.halfmoves << std::endl;
		}

		//after go => compute book moves async;
		//polyglot_thread = bookPonder.computePonderAsync(board);
		//board.makeMove(bestmove);
		//this->polyglotPonder = std::async(std::launch::async, &chess::PolyglotPonder::computePonder,std::ref(bookPonder),board);
		//board.undoMove();
        //Werkt niet: std::async(std::launch::async, &chess::SearchAgents::PVS::search<chess::BetterAgent, true>,std::ref(board),std::ref(bestmove), std::ref(ponder),std::ref(stopFlag));
	}
}

void UCI::UCI::newgame()
{
	//std::cout << "info new game" << std::endl;
	if (debug) logFile << "\nAnswer: " << "--" << std::endl;
}

void UCI::UCI::position(std::istringstream& is)
{
	//std::cout << "info position" << std::endl;
	chess::Move m; 
	std::string token, fen;
	is >> token;
	if (debug) logFile << " " << token;
	if (token == "startpos") {
		//board.reset();
		board = chess::ClassicBitBoard(chess::ClassicBitBoard::startpos);
		board = chess::ClassicBitBoard();
		is >> token;
		if (debug) logFile << " " << token;
	}
	else if (token == "fen") {
		while (is >> token && token != "moves") {
			if (debug) logFile << " " << token;
			fen += token + " "; //read fen, stop if keyword 'moves' is detected
		}
		board.set(fen);
	}
	else {
		return;
	}
	
	//read and make moves
	while (is >> token) {
		if (debug) logFile << " " << token;
		//TODO: error checking
		//std::cout << "info moving" << std::endl;
		m = board.movefromUCI(token);
		board.makeMove(m);
	}
	if (debug) logFile << std::endl;
}
