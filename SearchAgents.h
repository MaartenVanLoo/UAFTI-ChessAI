#include "ClassicBitBoard.h"
#include <iostream>
#include <sstream>
#include "TranspositionTable.h"
#include <chrono>
#include <unordered_map>
#include "Limits.h"
#include "Polyglot.h"
#include "EvalAgents.h"
#pragma once
namespace chess::SearchAgents{
	const int draw_value = 0;
	const int mate_Value = INT_MAX-2000;
	class Quiescence {
	public:
		static inline uint64_t nodes;
		static inline uint64_t threefold;
		static inline uint64_t mates;
		static inline uint64_t draws;

		template<class EvalAgent, bool side>
		static int quiescence(chess::ClassicBitBoard& brd) {
			std::vector<Move> moves;
			brd.generate_moves(moves);
			int value = side ? INT_MIN : INT_MAX;
			//terminal value check!
			if (moves.size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side ? -mate_Value-0 : mate_Value+0; //MATE, if depth is higer number = less deep = evaluated better
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (brd.isThreeFold()) {
				threefold++;
				return draw_value;
			}

			//TODO: Make a method that only generates captures!! (no more filtering!)
			std::vector<Move> captures;
			captures.reserve(moves.size());
			//filter only capture moves!
			for (int i = 0; i < moves.size(); i++) {
				if (side) {
					if (moves[i].to & brd.Black) {
						captures.push_back(moves[i]);
					}
				}
				else {
					if (moves[i].to & brd.White) {
						captures.push_back(moves[i]);
					}
				}

			}

			//if no capture moves => eval position
			if (captures.size() == 0) {
				return EvalAgent::template eval<side>(brd);
			}
			//now continue search with only capture moves
			if (side) {
				for (Move& m : captures) {
					brd.makeMove(m); nodes++;
					value = std::max(value, quiescence<EvalAgent, !side>(brd));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
				}
				return value;
			}
			else {
				for (Move& m : captures) {
					brd.makeMove(m); nodes++;
					value = std::min(value, quiescence<EvalAgent, !side>(brd));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
				}
				return value;
			}
		}

		template<class EvalAgent, bool side>
		static int quiescence(chess::ClassicBitBoard& brd, int alpha, int beta) {
			std::vector<Move> moves;
			brd.generate_moves(moves);
			int value = side ? INT_MIN : INT_MAX;
			//terminal value check!
			if (moves.size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side ? -mate_Value-0 : mate_Value+0; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (brd.isThreeFold()) {
				threefold++;
				return draw_value;
			}

			//TODO: Make a method that only generates captures!! (no more filtering!)
			std::vector<Move> captures;
			captures.reserve(moves.size());
			//filter only capture moves!
			for (int i = 0; i < moves.size(); i++) {
				if (side) {
					if (moves[i].to & brd.Black) {
						captures.push_back(moves[i]);
					}
				}
				else {
					if (moves[i].to & brd.White) {
						captures.push_back(moves[i]);
					}
				}

			}

			//if no capture moves => eval position
			if (captures.size() == 0) {
				return EvalAgent::template eval<side>(brd);
			}
			//now continue search with only capture moves
			if (side) {
				for (Move& m : captures) {
					brd.makeMove(m); nodes++;
					value = std::max(value, quiescence<EvalAgent, !side>(brd, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value >= beta) {
						break;
					}
					alpha = std::max(alpha, value);
				}
				return value;
			}
			else {
				for (Move& m : captures) {
					brd.makeMove(m); nodes++;
					value = std::min(value, quiescence<EvalAgent, !side>(brd, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value <= alpha) {
						break;
					}
					beta = std::min(beta, value);
				}
				return value;
			}
		}
	};
	class Minimax
	{		
	public:
		uint64_t nodes;
		uint64_t threefold;
		uint64_t mates;
		uint64_t draws;

		template<class EvalAgent, bool side>
		inline int minimax(chess::ClassicBitBoard& brd, int depth, Move& best) {
			nodes = 0;
			threefold = 0;
			mates = 0;
			draws = 0;

			int bestValue = side?INT_MIN:INT_MAX;
			if (depth == 0) {
				return EvalAgent::template eval<side>(brd);
			}
			std::vector<Move> moves;
			brd.generate_moves(moves);
			int value = side ? INT_MIN : INT_MAX;
			if (moves.size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side? -mate_Value-depth:mate_Value+depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (brd.isThreeFold()) {
				threefold++;
				//return draw_value; //Still do search to find best possible move, uci requires a valid move to be returned, no return on root of search lvl!
			}
			else {
				if (side) {
					for (Move& m : moves) {
						brd.makeMove(m); nodes++;
						value = std::max(value, Minimax::minimax<EvalAgent, !side>(brd, depth - 1));
						if (value > bestValue) {
							best.flags = m.flags;
							best.from = m.from;
							best.to = m.to;
							best.IsWhite = m.IsWhite;
							bestValue = value;
						}
						brd.undoMove();
						//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					}
				}
				else {
					for (Move& m : moves) {
						brd.makeMove(m); nodes++;
						value = std::min(value, Minimax::minimax< EvalAgent, !side>(brd, (depth - 1)));
						if (value < bestValue) {
							best.flags = m.flags;
							best.from = m.from;
							best.to = m.to;
							best.IsWhite = m.IsWhite;
							bestValue = value;
						}
						brd.undoMove();
						//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					}
				}
				
			}
			return value;

		}
		template<class EvalAgent, bool side>
		int minimax(chess::ClassicBitBoard& brd, int depth) {
			if (depth == 0) {
				return EvalAgent::template eval<side>(brd);
			}
			std::vector<Move> moves;
			brd.generate_moves(moves);
			int value = side ? INT_MIN : INT_MAX;
			if (moves.size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side ? -mate_Value -depth: mate_Value+depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (brd.isThreeFold()) {
				threefold++;
				return draw_value;
			}
			else {
				if (side) {
					for (Move const &m : moves) {
						brd.makeMove(m); nodes++;
						value = std::max(value, Minimax::minimax<EvalAgent, !side>(brd,depth-1));
						brd.undoMove();
						//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					}
				}
				else {
					for (Move const &m : moves) {
						brd.makeMove(m); nodes++;
						value = std::min(value, Minimax::minimax< EvalAgent, !side>(brd, (depth - 1)));
						brd.undoMove();
						//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					}
				}

			}
			return value;

		}

	private:
	};
	class AlphaBeta {
		Polyglot book;
	public:
		uint64_t nodes = 0;
		uint64_t twofold = 0;
		uint64_t threefold = 0;
		uint64_t mates = 0;
		uint64_t draws = 0;

		UCI::Limits limits;
		std::stringstream logFile;
		bool twoFoldEnabled = true;
		bool threeFoldEnabled = true;
		
		std::vector<std::vector<Move>> moves;
		
		template<class EvalAgent>
		int search(chess::ClassicBitBoard& board, Move& bestmove, Move& ponder) {
			limits.startSearch(board.side);

			//reset counters
			this->logFile.str("");
			this->nodes = 0;
			this->threefold = 0;
			this->twofold = 0;
			this->mates = 0;
			this->draws = 0;

			int value = 0;

			//first check polyglot table;
			uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
			if (book.getMove(key, bestmove, board)) {
				//std::cout << "info: theory hit" << std::endl;
				ponder = chess::Move();
				//get eval to return this value
				board.makeMove(bestmove);
				value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
				board.undoMove();
				return value;
			}

			//Next check early stop conditions:
			// 1.No possible moves = return empty move (should not be possible, this should be captured by gui)
			// 2.Only 1 possible move = make move
			// 3.Mate in 1;
			moves.resize(2);
			board.generate_moves(moves[0]);
			//No possible moves
			if (moves[0].size() == 0) {
				bestmove = Move();
				ponder = Move();
				return 0;
			}
			//1 possible move?
			if (moves[0].size() == 1) {
				bestmove = moves[0][0];
				ponder = chess::Move();
				board.makeMove(bestmove);
				value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
				board.undoMove();
				return value;
			}
			//mate in 1?
			for (auto& move : moves[0]) {
				board.makeMove(move);
				board.generate_moves(moves[1]);
				if (board.side) {
					if (moves[1].size() == 0 && board.isCheck<true>()) {
						//Mate in 1
						board.undoMove();
						bestmove = move;
						ponder = Move();
						return -mate_Value;//MATE
					}
				}
				else {
					if (moves[1].size() == 0 && board.isCheck<false>()) {
						//Mate in 1
						board.undoMove();
						bestmove = move;
						ponder = Move();
						return mate_Value; //MATE
					}
				}
				board.undoMove();
			}

			//Check if root node is 2 fold repetition. If root is already a 2 fold repetition => disable repetition check
			if (board.isTwoFold()) {
				this->twoFoldEnabled = false;
				if (board.isThreeFold()) {
					this->threeFoldEnabled = false;
				}
				else {
					this->threeFoldEnabled = true;
				}
			}
			else {
				this->twoFoldEnabled = true;
			}

			//Set default best move (avoid sending empty move)
			bestmove = moves[0][0]; //Already checked this value is > 1!
			ponder = Move();

			//No early stop conditions => start alpha beta search
			limits.nextItt();
			Move searchMove = bestmove; //no empty move!
			int searchValue;
			
			if (limits.depth == moves.size()) moves.resize((size_t)(moves.size() * 1.5) + 1);

			int alpha = INT_MIN;
			int beta = INT_MAX;
			if (board.side) {
				searchValue = alphabeta<EvalAgent, true>(board, limits.depth, alpha, beta, bestmove);
			}
			else {
				searchValue = alphabeta<EvalAgent, false>(board, limits.depth, alpha, beta, bestmove);
			}

			long long unsigned nps = this->nodes;
			if (limits.getElapsed() != 0) {
				nps = this->nodes * 1e3 / limits.getElapsed();
			}
			std::cout << "info depth " << limits.depth
				<< " nodes " << this->nodes
				<< " nps " << nps
				<< " mates " << this->mates
				<< " draws " << this->draws
				<< " score cp " << value
				<< " time " << limits.getElapsed()
				<< std::endl;
			this->logFile << "info depth " << limits.depth
				<< " nodes " << this->nodes
				<< " nps " << nps
				<< " mates " << this->mates
				<< " draws " << this->draws
				<< " score cp " << value
				<< " time " << limits.getElapsed()
				<< std::endl;
			return value;
		}
		

		template<class EvalAgent, bool side>
		int alphabeta(chess::ClassicBitBoard& brd, int depth, Move& best) {
			nodes = 0;
			threefold = 0;
			mates = 0;
			draws = 0;
			moves.resize(depth+1);

			int bestValue = side ? INT_MIN : INT_MAX;

			if (depth == 0) {
				return EvalAgent::template eval<side>(brd);
			}
			//std::vector<Move> moves;
			moves[depth].clear();
			brd.generate_moves(moves[depth]);
			int value = side ? INT_MIN : INT_MAX;
			if (moves[depth].size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side ? -mate_Value-depth : mate_Value+depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (brd.isThreeFold()) {
				threefold++;
				//return draw_value; //Still do search to find best possible move, uci requires a valid move to be returned, no return on root of search lvl!
			}
			int alpha = INT_MIN;
			int beta = INT_MAX;

			if (side) {
				for (Move& m : moves[depth]) {
					brd.makeMove(m); nodes++;
					value = std::max(value, alphabeta<EvalAgent, !side>(brd, depth - 1, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value > bestValue) {
						best.flags = m.flags;
						best.from = m.from;
						best.to = m.to;
						best.IsWhite = m.IsWhite;
						bestValue = value;
					}
					if (value >= beta) {
						break;
					}
					alpha = std::max(alpha, value);
				}
				return value;
			}
			else {
				for (Move& m : moves[depth]) {
					brd.makeMove(m); nodes++;
					value = std::min(value, alphabeta<EvalAgent, !side>(brd, depth - 1, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value < bestValue) {
						best.flags = m.flags;
						best.from = m.from;
						best.to = m.to;
						best.IsWhite = m.IsWhite;
						bestValue = value;
					}					
					if (value <= alpha) {
						break;
					}
					beta = std::min(beta, value);
				}
				return value;
			}			
			return 0;
		};
		template<class EvalAgent, bool side>
		int alphabeta(chess::ClassicBitBoard& brd, int depth, int alpha, int beta) {
			if (depth == 0) {
				return EvalAgent::template eval<side>(brd);
			}
			//std::vector<Move> moves;
			moves[depth].clear();
			brd.generate_moves(moves[depth]);
			int value = side ? INT_MIN : INT_MAX;
			if (moves[depth].size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side ? -mate_Value-depth : mate_Value+depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (brd.isThreeFold()) {
				threefold++;
				return draw_value;
			}
			if (side) {
				for (Move& m : moves[depth]) {
					brd.makeMove(m); nodes++;
					value = std::max(value, alphabeta<EvalAgent, !side>(brd, depth - 1, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value >= beta) {
						break;
					}
					alpha = std::max(alpha, value);
				}
				return value;
			}
			else {
				for (Move& m : moves[depth]) {
					brd.makeMove(m); nodes++;
					value = std::min(value, alphabeta<EvalAgent, !side>(brd, depth - 1, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value <= alpha) {
						break;
					}
					beta = std::min(beta, value);
				}
				return value;
			}
		}
		
		template<class EvalAgent, bool side, bool quienscenceSearch>
		int alphabeta(chess::ClassicBitBoard& brd, int depth, Move& best) {
			nodes = 0;
			threefold = 0;
			mates = 0;
			draws = 0;
			moves.resize(depth+1);
			
			int bestValue = side ? INT_MIN : INT_MAX;

			if (depth == 0) {
				if (quienscenceSearch) {
					return Quiescence::quiescence<EvalAgent, side>(brd, INT_MIN, INT_MAX);
				}
				else {
					return EvalAgent::template eval<side>(brd);
				}				
			}
			//std::vector<Move> moves;
			moves[depth].clear();
			brd.generate_moves(moves[depth]);
			int value = side ? INT_MIN : INT_MAX;
			if (moves[depth].size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side ? -mate_Value-depth : mate_Value+depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (brd.isThreeFold()) {
				threefold++;
				//return draw_value; //Still do search to find best possible move, uci requires a valid move to be returned, no return on root of search lvl!
			}		
			int alpha = INT_MIN;
			int beta = INT_MAX;
			if (side) {
				for (Move& m : moves) {
					brd.makeMove(m); nodes++;
					value = std::max(value, alphabeta<EvalAgent, !side, quienscenceSearch>(brd, depth - 1, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value > bestValue) {
						best.flags = m.flags;
						best.from = m.from;
						best.to = m.to;
						best.IsWhite = m.IsWhite;
						bestValue = value;
					}
					if (value >= beta) {
						break;
					}
					alpha = std::max(alpha, value);
				}
				return value;
			}
			else {
				for (Move& m : moves) {
					brd.makeMove(m); nodes++;
					value = std::min(value, alphabeta<EvalAgent, !side, quienscenceSearch>(brd, depth - 1, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value < bestValue) {
						best.flags = m.flags;
						best.from = m.from;
						best.to = m.to;
						best.IsWhite = m.IsWhite;
						bestValue = value;
					}					
					if (value <= alpha) {
						break;
					}
					beta = std::min(beta, value);
				}
				return value;
			}
			return 0;
		};
		template<class EvalAgent, bool side, bool quienscenceSearch>
		int alphabeta(chess::ClassicBitBoard& brd, int depth, int alpha, int beta) {
			if (depth == 0) {
				if (quienscenceSearch) {
					return Quiescence::quiescence<EvalAgent, side>(brd, INT_MIN, INT_MAX);
				}
				else {
					return EvalAgent::template eval<side>(brd);
				}
			}
			//std::vector<Move> moves;
			moves[depth].clear();
			brd.generate_moves(moves[depth]);
			int value = side ? INT_MIN : INT_MAX;
			if (moves[depth].size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side ? -mate_Value-depth : mate_Value+depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (brd.isThreeFold()) {
				threefold++;
				return draw_value;
			}
			if (side) {
				for (Move& m : moves) {
					brd.makeMove(m); nodes++;
					value = std::max(value, alphabeta<EvalAgent, !side, quienscenceSearch>(brd, depth - 1, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value >= beta) {
						break;
					}
					alpha = std::max(alpha, value);
				}
				return value;
			}
			else {
				for (Move& m : moves) {
					brd.makeMove(m); nodes++;
					value = std::min(value, alphabeta<EvalAgent, !side, quienscenceSearch>(brd, depth - 1, alpha, beta));
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value <= alpha) {
						break;
					}
					beta = std::min(beta, value);
				}
				return value;
			}
		}

	};
	class IttAlphaBeta {
		Polyglot book;		
	public:
		uint8_t searchID = 0; //increment every time a new search is started, overflow at the end => not a problem.
		uint64_t nodes;
		uint64_t threefold;
		uint64_t twofold;
		uint64_t mates;
		uint64_t draws; 
		uint64_t tableHits;
		std::vector<std::vector<Move>> moves;
		TTentry entry;
		TranspositionTable TTtable = TranspositionTable(1024);
		std::stringstream logFile;

		UCI::Limits limits;
		bool twoFoldEnabled = true;
		bool threeFoldEnabled = true;

		void setBook(Polyglot& book) {
			this->book = book;
		}
		bool getPonder(chess::ClassicBitBoard& brd, Move& ponder) {
			uint64_t key = ClassicBitBoard::HashUtil::createHash(brd);
			if (TTtable.contains(key)) {
				Move best = TTtable.move(key, this->searchID);
				brd.makeMove(best);
				uint64_t ponder_key = ClassicBitBoard::HashUtil::createHash(brd);
				if (TTtable.contains(ponder_key)) {
					ponder = TTtable.move(ponder_key, this->searchID);
					brd.undoMove();
					return true;
				}
				brd.undoMove();
			}
			return false;
		}
		bool getPV(chess::ClassicBitBoard& brd, std::vector<Move>& pv) {
			pv.clear();
			uint64_t key = ClassicBitBoard::HashUtil::createHash(brd);
			while (TTtable.contains(key, this->searchID)) {
				pv.push_back(TTtable.move(key, this->searchID));
				brd.makeMove(pv.back());
			}
			for (int i = 0; i < pv.size(); i++) {
				brd.undoMove();
			}

		}
		
		template<class EvalAgent>
		int search(chess::ClassicBitBoard& board, Move& bestmove, Move& ponder) {
			limits.startSearch(board.side);

			//reset counters
			this->logFile.str("");
			this->nodes = 0;
			this->threefold = 0;
			this->twofold = 0;
			this->mates = 0;
			this->draws = 0;
			this->tableHits = 0;
			this->TTtable.clearCollisions();
			
			int value = 0;

			//first check polyglot table;
			uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
			if (book.getMove(key, bestmove, board)) {
				//std::cout << "info: theory hit" << std::endl;
				ponder = chess::Move();
				//get eval to return this value
				board.makeMove(bestmove);
				value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
				board.undoMove();
				return value;
			}

			//Next check early stop conditions:
			// 1.No possible moves = return empty move (should not be possible, this should be captured by gui)
			// 2.Only 1 possible move = make move
			// 3.Mate in 1;
			moves.resize(2); 
			board.generate_moves(moves[0]);
			//No possible moves
			if (moves[0].size() == 0) {
				bestmove = Move();
				ponder = Move();
				return 0;
			}
			//1 possible move?
			if (moves[0].size() == 1) {
				bestmove = moves[0][0];
				ponder = chess::Move();
				board.makeMove(bestmove);
				value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
				board.undoMove();
				return value;
			}
			//mate in 1?
			for (auto& move : moves[0]) {
				board.makeMove(move);
				board.generate_moves(moves[1]);
				if (board.side) {
					if (moves[1].size() == 0 && board.isCheck<true>()) {
						//Mate in 1
						board.undoMove();
						bestmove = move;
						ponder = Move();
						return -mate_Value;//MATE
					}
				}
				else {
					if (moves[1].size() == 0 && board.isCheck<false>()) {
						//Mate in 1
						board.undoMove();
						bestmove = move;
						ponder = Move();
						return mate_Value; //MATE
					}
				}				
				board.undoMove();
			}
			
			//Check if root node is 2 fold repetition. If root is already a 2 fold repetition => disable 2-repetition check
			//      if root node is 3 fold repetition => also disable threefold
			if (board.isTwoFold()) {
				this->twoFoldEnabled = false;
				if (board.isThreeFold()) {
					this->threeFoldEnabled = false;
				}
				else {
					this->threeFoldEnabled = true;
				}
			}
			else {
				this->twoFoldEnabled = true;
			}

			//Set default best move (avoid sending empty move)
			bestmove = moves[0][0]; //Already checked this value is > 1!
			ponder = Move();
			
			//No early stop conditions => start alpha beta search
			limits.nextItt();
			Move searchMove = bestmove; //no empty move!
			int searchValue;
			int finalDepth = 0;
			for (int depth = 0; !limits.exeeded(depth); depth++) {
				if (depth == moves.size()) moves.resize((size_t)(moves.size() * 1.5) + 1);
                this->searchID++;

				int alpha = INT_MIN;
				int beta = INT_MAX;
				if (board.side) {
					searchValue = alphabeta<EvalAgent, true>(board, depth, alpha, beta, searchMove);
				}
				else {
					searchValue = alphabeta<EvalAgent, false>(board, depth, alpha, beta, searchMove);
				}
				if (limits.exeededTime()) {
					// alpha beta search was terminated prematurly => results not complete
					break;
				}
				bestmove = searchMove;
				value = searchValue;
				finalDepth = depth;
				getPonder(board, ponder);

				limits.nextItt();

				std::cout << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes << " score cp " << value << " pv " << bestmove << " " << ponder << std::endl;
				this->logFile << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes << " score cp " << value << " pv " << bestmove << " " << ponder << std::endl;
			}
			long long unsigned nps = this->nodes;
			if (limits.getElapsed() != 0) {
				nps = (uint64_t)(this->nodes * 1e3 / limits.getElapsed());
			}
			std::cout << "info depth " << finalDepth
				<< " nodes " << this->nodes
				<< " nps " << nps
				<< " mates " << this->mates
				<< " draws " << this->draws
				<< " score cp " << value
				<< " time " << limits.getElapsed()
				<< std::endl;
			this->logFile << "info depth " << finalDepth
				<< " nodes " << this->nodes
				<< " nps " << nps
				<< " mates " << this->mates
				<< " draws " << this->draws
				<< " score cp " << value
				<< " time " << limits.getElapsed()
				<< std::endl;
			return value;
		}
		
		/*
		template<class EvalAgent, bool side>
		int alphabeta(chess::ClassicBitBoard& brd, int depth, Move& best, Move& ponder) {
			this->limits = UCI::Limits();
			this->limits.startSearch(brd.side);

			this->logFile.clear();
			this->nodes = 0;
			this->threefold = 0;
			this->twofold = 0;
			this->mates = 0;
			this->draws = 0;
			this->tableHits = 0;
			this->moves.resize(depth + 1);
			this->TTtable.clearCollisions();

			int alpha = INT_MIN;
			int beta = INT_MAX;
			int value = 0;
			depth = depth > 0 ? depth: INT_MAX;
			int finalDepth = 0;
			int bestValue = 0;
			Move searchedMove;
			
			//if only 1 possible move => return move
			brd.generate_moves(moves[0]);
			if (moves[0].size() == 1) {
				best = moves[0][0];
				ponder = Move();	//empty move;
				return EvalAgent::template eval<side>(brd);
			}
			//Start search tree
			for (int itt = 1; itt <= depth; itt++) {
				if (itt == moves.size()) moves.resize((size_t)(moves.size() * 1.5) + 1);
				this->searchID++;

				value = alphabeta<EvalAgent,side>(brd, itt, alpha, beta, searchedMove);

				if (limits.exeededTime()) break;
				bestValue = value;
				finalDepth = itt;
				best = searchedMove;
				if (!getPonder(brd, ponder)) {
					ponder = Move();
				}
				std::cout << "info depth " << itt << " time " << limits.elapsedSinceStart() << " nodes " << this->nodes << " score cp " << bestValue << " pv " << best << " " << ponder << std::endl;
				this->logFile << "info depth " << itt << " time " << limits.elapsedSinceStart() << " nodes " << this->nodes << " score cp " << bestValue << " pv " << best << " " << ponder << std::endl;
			}
			std::cout << "info depth " << finalDepth
				<< " nodes " << this->nodes
				<< " nps " << (long long unsigned)(this->nodes * 1e6) / limits.elapsedMicrosecondsSinceStart()
				<< " mates " << this->mates
				<< " draws " << this->draws
				<< " score cp " << bestValue
				<< " time " << limits.elapsedSinceStart()
				<< std::endl;
			this->logFile << "info depth " << finalDepth
				<< " nodes " << this->nodes
				<< " nps " << (long long unsigned)(this->nodes * 1e6) / limits.elapsedMicrosecondsSinceStart()
				<< " mates " << this->mates
				<< " draws " << this->draws
				<< " score cp " << value
				<< " time " << limits.elapsedSinceStart()
				<< std::endl;

			return value;
		}
		*/	
	private:
		template<class EvalAgent, bool side>
		int alphabeta(chess::ClassicBitBoard& brd, int depth, int alpha, int beta, Move& best) {
			nodes++;
			//Note: probing table is more expansive compared to gain.
			//Perhapse if "eval" becomes more expensive later on probing might become better!
			//if (depth == 0) {
			//	//if (tablehit) return entry.eval; else
			//	return EvalAgent::template eval<side>(brd);
			//}
			//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->startTime).count();
			//if (elapsed > this->limitTime && this->limitTime > 0) return 0;

            //if time limit is exceeded cause the parent node to "CUT".
            //return "0" could cause undefined behavior (might be better compared to actual value)
            if (this->limits.exeededTime()) return side?INT_MAX:INT_MIN;

			//probe transposition table
			uint64_t key = ClassicBitBoard::HashUtil::createHash(brd);
			//bool tablehit = TTtable.get(key, entry, depth);
			bool tablehit = TTtable.contains(key);
			Move firstMove;
			if (tablehit) {
				tableHits++;
				//if (entry.depth >= depth) { //Already computed
				if (TTtable.depth(key, this->searchID) >= depth) { //Already computed
					//if (entry.type == TTtype::PV) { //Knuth's Type 1
					chess::TTtype type = TTtable.type(key, this->searchID);
					int eval = TTtable.eval(key, this->searchID);
					best = TTtable.move(key, this->searchID);
					if (type == TTtype::PV) { //Knuth's Type 1
						//best = entry.move;
						//return entry.eval;
						return eval;
					}
					else if (type == TTtype::CUT) { //Knuth's Type 2 = lowerbound
						//alpha = entry.eval;
						//best = entry.move;
						if (eval >= beta) { return eval; }
					}
					else if (type == TTtype::ALL) { //Knuth's Type 3 = upperbound
						//beta = entry.eval;
						//best = entry.move;
						if (eval <= alpha) { return eval; }
					}
					else {
						std::cout << "Something went wrong????" << std::endl;
					}
				}
				firstMove = TTtable.move(key, this->searchID);;
			}
			//std::vector<Move> moves;
			moves[depth].clear();
			brd.generate_moves(moves[depth]);
			if (moves[depth].size() == 0) //No more moves=terminal node
			{
				if (brd.isCheck<side>()) {
					mates++;
					return side ? -mate_Value - depth : mate_Value + depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			/*if (brd.isThreeFold()) {
				threefold++;
				return draw_value;
			}*/
			if (this->twoFoldEnabled && brd.isTwoFold()) {//Twofold repetition => not making 'progress' => count as a draw
				twofold++;
				return draw_value;
			}
			else if (this->threeFoldEnabled && brd.isThreeFold()) {
				threefold++;
				return draw_value;
			}
			//set 'PV move first':
			if (tablehit) {
				brd.sort<side>(moves[depth], firstMove);
				/*for (int i = 0; i < moves[depth].size(); i++) {
					if (firstMove == moves[depth][i]) {
						std::swap(moves[depth][i], moves[depth][0]);
						break;
					}
				}*/
			}
			else {
				brd.sort<side>(moves[depth]);
			}

			int in_alpha = alpha;
			int in_beta = beta;

			if (side) {
				int bestValue = INT_MIN;
				int value = INT_MIN;
				for (Move& m : moves[depth]) {
					brd.makeMove(m);
					if (depth <= 1) {
						value = std::max(value, EvalAgent::template eval<side>(brd));
						nodes++;
					}
					else {
						value = std::max(value, alphabeta<EvalAgent, !side>(brd, depth - 1, alpha, beta, firstMove));
					}
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value > bestValue) {
						best.flags = m.flags;
						best.from = m.from;
						best.to = m.to;
						best.IsWhite = m.IsWhite;
						bestValue = value;
					}
					if (value >= beta) {
						break;
					}
					alpha = std::max(alpha, value);
				}
				if (value > in_alpha && value < in_beta) {
					TTtable.update(key, this->searchID, TTtype::PV, value, depth, best);
				}
				else if (value >= in_beta) {
					TTtable.update(key, this->searchID, TTtype::CUT, value, depth, best);
				}
				else if (value <= in_alpha) {
					TTtable.update(key, this->searchID, TTtype::ALL, value, depth, best);
				}
				return value;
			}
			else {
				int bestValue = INT_MAX;
				int value = INT_MAX;
				for (Move& m : moves[depth]) {
					brd.makeMove(m); nodes++;
					if (depth <= 1) {
						value = std::min(value, EvalAgent::template eval<side>(brd));
					}
					else {
						value = std::min(value, alphabeta<EvalAgent, !side>(brd, depth - 1, alpha, beta, firstMove));
					}
					brd.undoMove();
					//std::cout << brd.MovetoSAN(m) << " " << value << std::endl;
					if (value < bestValue) {
						best.flags = m.flags;
						best.from = m.from;
						best.to = m.to;
						best.IsWhite = m.IsWhite;
						//store ponder move as well
						bestValue = value;
					}
					if (value <= alpha) {
						break;
					}
					beta = std::min(beta, value);
				}
				if (value > in_alpha && value < in_beta) {
					TTtable.update(key, this->searchID, TTtype::PV, value, depth, best);
				}
				else if (value >= in_beta) {
					//note: 'black to move' = minimizing => upper bound = all node
					TTtable.update(key, this->searchID, TTtype::ALL, value, depth, best);
				}
				else if (value <= in_alpha) {
					//note: 'black to move' = minimizing => lower bound = cut node
					TTtable.update(key, this->searchID, TTtype::CUT, value, depth, best);
				}
				return value;
			}
		}

};


};

