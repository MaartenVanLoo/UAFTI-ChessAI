#include "../chess/ClassicBitBoard.h"
#include <iostream>
#include <sstream>
#include "TranspositionTable.h"
#include <chrono>
#include <unordered_map>
#include "../UCI/Limits.h"
#include "../chess/Polyglot.h"
#include "EvalAgents.h"
#pragma once
namespace chess::SearchAgents{
	const int draw_value = 0;
	const int mate_Value = INT_MAX-2000;

	int isMate(int value);
	int mateIn(int searchdepth, int value);
	class Quiescence {
	private:
	    //used as default parameter;
	    static Move staticmove;
	public:
	    //counters;
        uint8_t searchID = 0; //increment every time a new search is started, overflow at the end => not a problem.
        uint64_t nodes;
        uint64_t threefold;
        uint64_t twofold;
        uint64_t mates;
        uint64_t draws;
        uint64_t tableHits;

        //help objects
        std::vector<std::vector<Move>> moves;

        //Pointer to calling agent objects
        TranspositionTable* TTtable = nullptr;
        UCI::Limits* limits = nullptr;

        bool twoFoldEnabled = true;
        bool threeFoldEnabled = true;


        Quiescence(TranspositionTable *TTtable, UCI::Limits *limits) :TTtable(TTtable), limits(limits){}

        /// <summary>
        /// Call at start of search to make sure non of the pointers have been changed by the UCI engine;
        /// </summary>
        void setPointers(TranspositionTable *TTtable, UCI::Limits *limits){
            TTtable = TTtable;
            limits = limits;
        }
        void setRepetition(bool twoFoldEnabled, bool threeFoldEnabled){
            this->twoFoldEnabled = twoFoldEnabled;
            this->threeFoldEnabled = threeFoldEnabled;
        }
        void setSearchID(uint8_t searchID){this->searchID = searchID;}
        void resetCounter(){
            this->nodes = 0;
            this->threefold = 0;
            this->twofold = 0;
            this->mates = 0;
            this->draws = 0;
            this->tableHits = 0;
        }


        /// <summary>
        /// Qsearch. Depth is needed to create a correct mate value, also takes alpha and beta into account for pruning
        /// Has acces to the same TTtable as calling search agent! Also limits can be accessed
        /// ply is only used internaly for reusing memory => do not use when calling from other search agent
        /// qsearch is based on the normal "alpha beta" three;
        /// </summary>
        template <class EvalAgent,bool side>
        int qsearch(chess::ClassicBitBoard& board, int depth, int alpha, int beta, int ply = 0, Move& best = staticmove){
            if (moves.size() <= ply ){ moves.resize((int)(moves.size() * 1.5 + 1)); } //make sure "moves" is big enough
            this->nodes++;

            //if time limis is exceeded cuase the parent node to "CUT".
            //return "0" could cause undefined behavior (might be better compared to actual value)
            if (this->limits->exceededTime()) {
                std::cout << "time exceeded" << std::endl;
                return side?INT_MAX : INT_MIN;
            }

            //probe transposition table
            uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
            bool tablehit = TTtable->contains(key);
            Move firstMove;
            if (tablehit) {
                //Every eval is oké => if eval from parant search (Depth > 0) => good, contains more information
                // if eval from other qsearch (depth == -1) => also good, qsearch always searches until no more caputers thus reagardless of the current depth  the same three will be expanded
                tableHits++;
                firstMove = TTtable->move(key, this->searchID);
                // Already computed => note, qsearch depth can become negative! => negative depth = used for all
                // qsearch entries. Note => any entry for regular search outperformes the qsearch.
                chess::TTtype type = TTtable->type(key, this->searchID);
                int eval = TTtable->eval(key, this->searchID);
                if (type == TTtype::PV) { //Knuth's Type 1
                    return eval;
                }
                else if (type == TTtype::CUT) { //Knuth's Type 2 = lower bound
                    if (side) {
                        if (eval >= beta) { return eval; }
                    }else{
                        if (eval <= alpha){ return eval; }
                    }
                }
                else if (type == TTtype::ALL) { //Knuth's Type 3 = upper bound
                    if (side){
                        if (eval <= alpha) { return eval; }
                    }else{
                        if (eval >= beta) { return eval; }
                    }
                }
                else {
                    std::cout << "Something went wrong????" << std::endl;
                }
            }

            //check if node is terminal
            board.generate_capture_moves(moves[ply]);
            if (moves[ply].empty()) //No more moves=terminal node
            {
                //Terminal node in q search => do evaluation;
                bool IsDraw;
                bool IsMate;
                board.side?board.isTerminal<true>(IsDraw,IsMate):board.isTerminal<false>(IsDraw,IsMate);
                if (IsMate){
                    mates++;
                    return side? -mate_Value-depth:mate_Value+depth; //MATE
                }else if (IsDraw){
                    draws++;
                    return draw_value; //DRAW
                }else{
                    return EvalAgent::template eval<!side>(board);
                }
            }

            //Repetition
            if (this->twoFoldEnabled && board.isTwoFold()) {	// Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }

            //set 'PV move first':
            if (tablehit) {
                board.sort<side>(moves[ply], firstMove);
            }
            else {
                board.sort<side>(moves[ply]);
            }

            int in_alpha = alpha;
            int in_beta = beta;

            //Start actual search
            if (side) {
                int bestValue = INT_MIN;
                int value = INT_MIN;
                for (Move& m : moves[depth]) {
                    board.makeMove(m);
                    value = std::max(value, qsearch<EvalAgent, !side>(board, depth - 1, alpha, beta, ply + 1,firstMove));
                    board.undoMove();
                    //std::cout << board.MovetoSAN(m) << " " << value << std::endl;
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
                    TTtable->update(key, this->searchID, TTtype::PV, value, -1, best);
                }
                else if (value >= in_beta) {
                    TTtable->update(key, this->searchID, TTtype::CUT, value, -1, best);
                }
                else if (value <= in_alpha) {
                    TTtable->update(key, this->searchID, TTtype::ALL, value, -1, best);
                }
                return value;
            }
            else {
                int bestValue = INT_MAX;
                int value = INT_MAX;
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    value = std::min(value, qsearch<EvalAgent, !side>(board, depth - 1, alpha, beta, ply + 1,firstMove));
                    board.undoMove();
                    //std::cout << board.MovetoSAN(m) << " " << value << std::endl;
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
                if (value > in_alpha && value < in_beta) {
                    TTtable->update(key, this->searchID, TTtype::PV, value, -1, best);
                }
                else if (value >= in_beta) {
                    //note: 'black to move' = minimizing => upper bound = all node
                    TTtable->update(key, this->searchID, TTtype::ALL, value, -1, best);
                }
                else if (value <= in_alpha) {
                    //note: 'black to move' = minimizing => lower bound = cut node
                    TTtable->update(key, this->searchID, TTtype::CUT, value, -1, best);
                }
                return value;
            }
        }
	};

	class Minimax
	{
        Polyglot book;
    public:
        // counters
        uint64_t nodes;
        uint64_t twofold;
        uint64_t threefold;
        uint64_t mates;
        uint64_t draws;


        //help objects
        std::vector<std::vector<Move>> moves;
        std::stringstream logFile;

        //UCI object to handle search limits
        UCI::Limits limits;

        bool twoFoldEnabled = true;
        bool threeFoldEnabled = true;

        void setBook(Polyglot& polyglot) {
            this->book = polyglot;
        }

        template<class EvalAgent>
        int search(chess::ClassicBitBoard& board, int depth, Move& bestMove, Move& ponder){
            this->limits.depth = depth;
            return this->search<EvalAgent>(board, bestMove, ponder);
        }
        template<class EvalAgent>
        int search(chess::ClassicBitBoard& board, Move& bestMove, Move& ponder){
            limits.startSearch(board.side);

            //reset counters
            this->nodes = 0;
            this->twofold = 0;
            this->threefold = 0;
            this->mates = 0;
            this->draws = 0;

            //Initialize return value;
            int value = 0;

            //Set ponder move to "empty move (not calculated in minimax)
            ponder = Move();

            //first check polyglot table;
            uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
            if (book.getMove(key, bestMove, board)) {
                //get eval to return this value
                board.makeMove(bestMove);
                value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
                board.undoMove();
                return value;
            }

            //Next check early stop conditions:
            // 1.No possible moves = return empty move (should not be possible, this should be captured by gui)
            // 2.Only 1 possible move = make move
            // 3.Mate in 1;
            moves.resize(std::max(2,limits.depth+1));
            board.generate_moves(moves[0]);

            //No possible moves
            if (moves[0].empty()) {
                bestMove = Move();
                ponder = Move();
                return 0;
            }
            //1 possible move?
            if (moves[0].size() == 1) {
                bestMove = moves[0][0];
                ponder = chess::Move();
                board.makeMove(bestMove);
                value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
                board.undoMove();
                return value;
            }
            //mate in 1?
            for (auto& move : moves[0]) {
                board.makeMove(move);
                board.generate_moves(moves[1]);
                if (board.side) {
                    if (moves[1].empty() && board.isCheck<true>()) {
                        //Mate in 1
                        board.undoMove();
                        bestMove = move;
                        ponder = Move();
                        return -mate_Value;//MATE
                    }
                }
                else {
                    if (moves[1].empty() && board.isCheck<false>()) {
                        //Mate in 1
                        board.undoMove();
                        bestMove = move;
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
            bestMove = moves[0][0]; //Already checked this value is >= 1!
            //No early stop condition => start minimax search
            if (board.side){
                value = minimax<EvalAgent,true>(board, limits.depth, bestMove);
            }else{
                value = minimax<EvalAgent,false>(board, limits.depth, bestMove);
            }
            long long unsigned nps = this->nodes;
            if (limits.getElapsed() > 0) {
                nps = (uint64_t)(this->nodes * 1e3 / limits.getElapsed());
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
	private:
		template<class EvalAgent, bool side>
		int minimax(chess::ClassicBitBoard& board, int depth, Move& best) {
			int bestValue = side?INT_MIN:INT_MAX;
			if (depth == 0) {
				return EvalAgent::template eval<side>(board);
			}
			moves[depth].clear();
			board.generate_moves(moves[depth]);
			int value = side ? INT_MIN : INT_MAX;
			if (moves[depth].empty()) //No more moves=terminal node
			{
				if (board.isCheck<side>()) {
					mates++;
					return side? -mate_Value-depth:mate_Value+depth; //MATE
				}
				else {
					draws++;
					return draw_value; //DRAW
				}
			}
            if (this->twoFoldEnabled && board.isTwoFold()) {//Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }
            if (side) {
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    value = std::max(value, minimax<EvalAgent, !side>(board, depth - 1));
                    if (value > bestValue) {
                        best.flags = m.flags;
                        best.from = m.from;
                        best.to = m.to;
                        best.IsWhite = m.IsWhite;
                        bestValue = value;
                    }
                    board.undoMove();
                }
                return value;
            }
            else {
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    value = std::min(value, minimax<EvalAgent, !side>(board, depth - 1));
                    if (value < bestValue) {
                        best.flags = m.flags;
                        best.from = m.from;
                        best.to = m.to;
                        best.IsWhite = m.IsWhite;
                        bestValue = value;
                    }
                    board.undoMove();
                }
                return value;
            }
		}
		template<class EvalAgent, bool side>
		int minimax(chess::ClassicBitBoard& board, int depth) {
            if (depth == 0) {
                return EvalAgent::template eval<side>(board);
            }
            moves[depth].clear();
            board.generate_moves(moves[depth]);
            int value = side ? INT_MIN : INT_MAX;
            if (moves[depth].empty()) //No more moves=terminal node
            {
                if (board.isCheck<side>()) {
                    mates++;
                    return side? -mate_Value-depth:mate_Value+depth; //MATE
                }
                else {
                    draws++;
                    return 0; //DRAW
                }
            }
            if (this->twoFoldEnabled && board.isTwoFold()) {//Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }
            if (side) {
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    value = std::max(value, minimax<EvalAgent, !side>(board, depth - 1));
                    board.undoMove();
                }
            }
            else {
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    value = std::min(value, minimax<EvalAgent, !side>(board, depth - 1));
                    board.undoMove();
                }
            }
            return value;
		}

	private:
	};
	class AlphaBeta {
		Polyglot book;
	public:
        // counters
        uint64_t nodes;
        uint64_t threefold;
        uint64_t twofold;
        uint64_t mates;
        uint64_t draws;

        //help objects
        std::vector<std::vector<Move>> moves;
        std::stringstream logFile;

        //UCI object to handle search limits
        UCI::Limits limits;
        bool twoFoldEnabled = true;
        bool threeFoldEnabled = true;

        void setBook(Polyglot& polyglot) {
            this->book = polyglot;
        }

		template<class EvalAgent>
		int search(chess::ClassicBitBoard& board, int depth, Move& bestMove, Move& ponder){
		    this->limits.depth = depth;
		    return this->search<EvalAgent>(board, bestMove, ponder);
		}
		template<class EvalAgent>
		int search(chess::ClassicBitBoard& board, Move& bestMove, Move& ponder) {
			limits.startSearch(board.side);

			//reset counters
			this->logFile.str("");
			this->nodes = 0;
			this->threefold = 0;
			this->twofold = 0;
			this->mates = 0;
			this->draws = 0;

            //Initialize return value;
			int value;

            //Set ponder move to "empty move (not calculated in alpha beta)
            ponder = Move();

			//first check polyglot table;
			uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
			if (book.getMove(key, bestMove, board)) {
				//std::cout << "info: theory hit" << std::endl;
				ponder = chess::Move();
				//get eval to return this value
				board.makeMove(bestMove);
				value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
				board.undoMove();
				return value;
			}

			//Next check early stop conditions:
			// 1.No possible moves = return empty move (should not be possible, this should be captured by gui)
			// 2.Only 1 possible move = make move
			// 3.Mate in 1;
			moves.resize(std::max(2,limits.depth+1));
			board.generate_moves(moves[0]);
			//No possible moves
			if (moves[0].empty()) {
                bestMove = Move();
				ponder = Move();
				return 0;
			}
			//1 possible move?
			if (moves[0].size() == 1) {
                bestMove = moves[0][0];
				ponder = chess::Move();
				board.makeMove(bestMove);
				value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
				board.undoMove();
				return value;
			}
			//mate in 1?
			for (auto& move : moves[0]) {
				board.makeMove(move);
				board.generate_moves(moves[1]);
				if (board.side) {
					if (moves[1].empty() && board.isCheck<true>()) {
						//Mate in 1
						board.undoMove();
                        bestMove = move;
						ponder = Move();
						return -mate_Value;//MATE
					}
				}
				else {
					if (moves[1].empty() && board.isCheck<false>()) {
						//Mate in 1
						board.undoMove();
                        bestMove = move;
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
			bestMove = moves[0][0]; //Already checked this value is >= 1!

			//No early stop conditions => start alpha beta search
			int alpha = INT_MIN;
			int beta = INT_MAX;
			if (board.side) {
				value = alphabeta<EvalAgent, true>(board, limits.depth, alpha, beta, bestMove);
			}
			else {
				value = alphabeta<EvalAgent, false>(board, limits.depth, alpha, beta, bestMove);
			}
			long long unsigned nps = this->nodes;
			if (limits.getElapsed() > 0) {
				nps = (uint64_t)(this->nodes * 1e3 / limits.getElapsed());
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

	private:
		template<class EvalAgent, bool side>
        int alphabeta(chess::ClassicBitBoard& board, int depth, int alpha, int beta, Move& bestMove) {
			int bestValue = side ? INT_MIN : INT_MAX;

			if (depth == 0) {
				return EvalAgent::template eval<side>(board);
			}
			board.generate_moves(moves[depth]);
			int value = side ? INT_MIN : INT_MAX;
			if (moves[depth].empty()) //No more moves=terminal node
			{
				if (board.isCheck<side>()) {
					mates++;
					return side ? -mate_Value-depth : mate_Value+depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
            if (this->twoFoldEnabled && board.isTwoFold()) {	// Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }
			if (side) {
				for (Move& m : moves[depth]) {
					board.makeMove(m); nodes++;
					value = std::max(value, alphabeta<EvalAgent, !side>(board, depth - 1, alpha, beta));
					board.undoMove();

					if (value > bestValue) {
                        bestMove.flags = m.flags;
                        bestMove.from = m.from;
                        bestMove.to = m.to;
                        bestMove.IsWhite = m.IsWhite;
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
					board.makeMove(m); nodes++;
					value = std::min(value, alphabeta<EvalAgent, !side>(board, depth - 1, alpha, beta));
					board.undoMove();

					if (value < bestValue) {
                        bestMove.flags = m.flags;
                        bestMove.from = m.from;
                        bestMove.to = m.to;
                        bestMove.IsWhite = m.IsWhite;
						bestValue = value;
					}
					if (value <= alpha) {
						break;
					}
					beta = std::min(beta, value);
				}
                return value;
			}
		};
		template<class EvalAgent, bool side>
		int alphabeta(chess::ClassicBitBoard& board, int depth, int alpha, int beta) {
            if (depth == 0) {
                return EvalAgent::template eval<side>(board);
            }
            board.generate_moves(moves[depth]);
            int value = side ? INT_MIN : INT_MAX;
            if (moves[depth].empty()) //No more moves=terminal node
            {
                if (board.isCheck<side>()) {
                    mates++;
                    return side ? -mate_Value-depth : mate_Value+depth; //MATE
                }
                else {
                    draws++;
                    return 0; //DRAW
                }
            }
            if (this->twoFoldEnabled && board.isTwoFold()) {	// Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }
            if (side) {
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    value = std::max(value, alphabeta<EvalAgent, !side>(board, depth - 1, alpha, beta));
                    board.undoMove();

                    if (value >= beta) {
                        break;
                    }
                    alpha = std::max(alpha, value);
                }
                return value;
            }
            else {
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    value = std::min(value, alphabeta<EvalAgent, !side>(board, depth - 1, alpha, beta));
                    board.undoMove();

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
	    // counters
		uint8_t searchID = 0; //increment every time a new search is started, overflow at the end => not a problem.
		uint64_t nodes;
		uint64_t threefold;
		uint64_t twofold;
		uint64_t mates;
		uint64_t draws; 
		uint64_t tableHits;

		//help objects
		std::vector<std::vector<Move>> moves;
		TTentry entry;
		TranspositionTable TTtable = TranspositionTable(1024);
		std::stringstream logFile;

		//UCI object to handle search limits
		UCI::Limits limits;
		bool twoFoldEnabled = true;
		bool threeFoldEnabled = true;

		void setBook(Polyglot& polyglot) {
			this->book = polyglot;
		}
		bool getPonder(chess::ClassicBitBoard& board, Move& ponder) {
			uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
			if (TTtable.contains(key)) {
				Move best = TTtable.move(key, this->searchID);
				board.makeMove(best);
				uint64_t ponder_key = ClassicBitBoard::HashUtil::createHash(board);
				if (TTtable.contains(ponder_key)) {
					ponder = TTtable.move(ponder_key, this->searchID);
					board.undoMove();
					return true;
				}
				board.undoMove();
			}
			return false;
		}
		bool getPV(chess::ClassicBitBoard& board, std::vector<Move>& pv) {
            pv.clear();
            uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
            while (TTtable.contains(key, this->searchID)) {
                pv.push_back(TTtable.move(key, this->searchID));
                board.makeMove(pv.back());
            }
            for (int i = 0; i < pv.size(); i++) {
                board.undoMove();
            }
        }

        template<class EvalAgent>
        int search(chess::ClassicBitBoard& board, int depth, Move& bestMove, Move& ponder){
		    this->limits.depth= depth;
		    return this->search<EvalAgent>(board, bestMove, ponder);
		}
		template<class EvalAgent>
		int search(chess::ClassicBitBoard& board, Move& bestMove, Move& ponder) {
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

			//Initialize return value;
			int value = 0;
			
			//first check polyglot table;
			uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
			if (book.getMove(key, bestMove, board)) {
				ponder = chess::Move();
				//get eval to return this value
				board.makeMove(bestMove);
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
			if (moves[0].empty()) {
                bestMove = Move();
				ponder = Move();
				return 0;
			}
			//1 possible move?
			if (moves[0].size() == 1) {
                bestMove = moves[0][0];
				ponder = chess::Move();
				board.makeMove(bestMove);
				value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
				board.undoMove();
				return value;
			}
			//mate in 1?
			for (auto& move : moves[0]) {
				board.makeMove(move);
				board.generate_moves(moves[1]);
				if (board.side) {
					if (moves[1].empty() && board.isCheck<true>()) {
						//Mate in 1
						board.undoMove();
                        bestMove = move;
						ponder = Move();
						return -mate_Value;//MATE
					}
				}
				else {
					if (moves[1].empty() && board.isCheck<false>()) {
						//Mate in 1
						board.undoMove();
                        bestMove = move;
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
			bestMove = moves[0][0]; //Already checked this value is >= 1!
			ponder = Move();
			
			//No early stop conditions => start alpha beta search
			limits.nextItt();
			Move searchMove = bestMove; //no empty move!
			int searchValue;
			int finalDepth = 0;
			for (int depth = 1; !limits.exceeded(depth); depth++) {
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
				if (limits.exceededTime()) {
					// alpha beta search was terminated prematurely => results not complete
					break;
				}
                bestMove = searchMove;
				value = searchValue;
				finalDepth = depth;
				getPonder(board, ponder);

				limits.nextItt();

				std::cout << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes << " score cp " << value << " pv " << bestMove << " " << ponder << std::endl;
				this->logFile << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes << " score cp " << value << " pv " << bestMove << " " << ponder << std::endl;
			}
			long long unsigned nps = this->nodes;
			if (limits.getElapsed() > 0) {
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

	private:
		template<class EvalAgent, bool side>
		int alphabeta(chess::ClassicBitBoard& board, int depth, int alpha, int beta, Move& best) {
			nodes++;
			//Note: probing table is more expansive compared to gain.
			//Perhapse if "eval" becomes more expensive later on probing might become better!
			//if (depth == 0) {
			//	//if (tablehit) return entry.eval; else
			//	return EvalAgent::template eval<side>(board);
			//}
			//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->startTime).count();
			//if (elapsed > this->limitTime && this->limitTime > 0) return 0;

            //if time limit is exceeded cause the parent node to "CUT".
            //return "0" could cause undefined behavior (might be better compared to actual value)
            if (this->limits.exceededTime()) return side ? INT_MAX : INT_MIN;

			//probe transposition table
			uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
			//bool tablehit = TTtable.get(key, entry, depth);
			bool tablehit = TTtable.contains(key);
			Move firstMove;
			if (tablehit) {
				tableHits++;
                firstMove = TTtable.move(key, this->searchID);
				//if (entry.depth >= depth) { //Already computed
				if (TTtable.depth(key, this->searchID) >= depth) { //Already computed
					//if (entry.type == TTtype::PV) { //Knuth's Type 1
					chess::TTtype type = TTtable.type(key, this->searchID);
					int eval = TTtable.eval(key, this->searchID);
					best = firstMove;
					if (type == TTtype::PV) { //Knuth's Type 1
						return eval;
					}
					else if (type == TTtype::CUT) { //Knuth's Type 2 = lower bound
						 if (side) {
                            if (eval >= beta) { return eval; }
                        }else{
                            if (eval <= alpha){ return eval; }
                        }
					}
					else if (type == TTtype::ALL) { //Knuth's Type 3 = upper bound
						 if (side){
                            if (eval <= alpha) { return eval; }
                        }else{
                            if (eval >= beta) { return eval; }
                        }
					}
					else {
						std::cout << "Something went wrong????" << std::endl;
					}
				}
			}
			board.generate_moves(moves[depth]);
			if (moves[depth].empty()) //No more moves=terminal node
			{
				if (board.isCheck<side>()) {
					mates++;
					return side ? -mate_Value - depth : mate_Value + depth; //MATE
				}
				else {
					draws++;
					return 0; //DRAW
				}
			}
			if (this->twoFoldEnabled && board.isTwoFold()) {	// Twofold repetition => not making 'progress' => count as a draw
				twofold++;
				return draw_value;
			}
			else if (this->threeFoldEnabled && board.isThreeFold()) {
				threefold++;
				return draw_value;
			}
			//set 'PV move first':
			if (tablehit) {
				board.sort<side>(moves[depth], firstMove);
			}
			else {
				board.sort<side>(moves[depth]);
			}

			int in_alpha = alpha;
			int in_beta = beta;

			if (side) {
				int bestValue = INT_MIN;
				int value = INT_MIN;
				for (Move& m : moves[depth]) {
					board.makeMove(m);
					if (depth <= 1) {
						value = std::max(value, EvalAgent::template eval<!side>(board));
						nodes++;
					}
					else {
						value = std::max(value, alphabeta<EvalAgent, !side>(board, depth - 1, alpha, beta, firstMove));
					}
					board.undoMove();
					//std::cout << board.MovetoSAN(m) << " " << value << std::endl;
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
					board.makeMove(m); nodes++;
					if (depth <= 1) {
						value = std::min(value, EvalAgent::template eval<!side>(board));
					}
					else {
						value = std::min(value, alphabeta<EvalAgent, !side>(board, depth - 1, alpha, beta, firstMove));
					}
					board.undoMove();
					//std::cout << board.MovetoSAN(m) << " " << value << std::endl;
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

    class PVS{
        Polyglot book;
    public:
        // counters
        uint8_t searchID = 0; //increment every time a new search is started, overflow at the end => not a problem.
        uint64_t nodes;
        uint64_t threefold;
        uint64_t twofold;
        uint64_t mates;
        uint64_t draws;
        uint64_t tableHits;
        uint64_t research;

        //help objects
        std::vector<std::vector<Move>> moves;
        TTentry entry;
        TranspositionTable TTtable = TranspositionTable(1024);
        std::stringstream logFile;

        //UCI object to handle search limits
        UCI::Limits limits;
        bool twoFoldEnabled = true;
        bool threeFoldEnabled = true;

        void setBook(Polyglot& polyglot) {
            this->book = polyglot;
        }
        bool getPonder(chess::ClassicBitBoard& board, Move& ponder) {
            uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
            if (TTtable.contains(key)) {
                Move best = TTtable.move(key, this->searchID);
                board.makeMove(best);
                uint64_t ponder_key = ClassicBitBoard::HashUtil::createHash(board);
                if (TTtable.contains(ponder_key)) {
                    ponder = TTtable.move(ponder_key, this->searchID);
                    board.undoMove();
                    return true;
                }
                board.undoMove();
            }
            return false;
        }

        template<class EvalAgent>
        int search(chess::ClassicBitBoard& board, int depth, Move& bestMove, Move& ponder){
            this->limits.depth= depth;
            return this->search<EvalAgent>(board, bestMove, ponder);
        }
        template<class EvalAgent>
        int search(chess::ClassicBitBoard& board, Move& bestMove, Move& ponder) {
            limits.startSearch(board.side);

            //reset counters
            this->logFile.clear();
            this->logFile.str("");
            this->nodes = 0;
            this->threefold = 0;
            this->twofold = 0;
            this->mates = 0;
            this->draws = 0;
            this->tableHits = 0;
            this->research = 0;
            this->TTtable.clearCollisions();

            //Initialize return value;
            int value = 0;

            //first check polyglot table;
            uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
            if (book.getMove(key, bestMove, board)) {
                //std::cout << "info: theory hit" << std::endl;
                ponder = chess::Move();
                //get eval to return this value
                board.makeMove(bestMove);
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
            if (moves[0].empty()) {
                bestMove = Move();
                ponder = Move();
                return 0;
            }
            //1 possible move?
            if (moves[0].size() == 1) {
                bestMove = moves[0][0];
                ponder = chess::Move();
                board.makeMove(bestMove);
                value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
                board.undoMove();
                return value;
            }
            //mate in 1?
            for (auto& move : moves[0]) {
                board.makeMove(move);
                board.generate_moves(moves[1]);
                if (board.side) {
                    if (moves[1].empty() && board.isCheck<true>()) {
                        //Mate in 1
                        board.undoMove();
                        bestMove = move;
                        ponder = Move();
                        return -mate_Value;//MATE
                    }
                }
                else {
                    if (moves[1].empty() && board.isCheck<false>()) {
                        //Mate in 1
                        board.undoMove();
                        bestMove = move;
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
            bestMove = moves[0][0]; //Already checked this value is >= 1!
            ponder = Move();

            //No early stop conditions => start alpha beta search
            limits.nextItt();
            Move searchMove = bestMove; //no empty move!
            int searchValue;
            int finalDepth = 0;
            for (int depth = 1; !limits.exceeded(depth); depth++) {
                if (depth == moves.size()) moves.resize((size_t)(moves.size() * 1.5) + 1);
                this->searchID++;

                int alpha = INT_MIN;
                int beta = INT_MAX;
                if (board.side)
                    searchValue = pvs<EvalAgent,true>(board, depth, alpha, beta, searchMove);
                else{
                    searchValue = pvs<EvalAgent,false>(board, depth, alpha, beta, searchMove);
                }

                if (limits.exceededTime() || searchValue == INT_MAX || searchValue == INT_MIN) {
                    // alpha beta search was terminated prematurely => results not complete
                    break;
                }

                bestMove = searchMove;
                value = board.side?searchValue:-searchValue;
                finalDepth = depth;
                getPonder(board, ponder);
                //is_mate = isMate(value);
                //mate_in = mateIn(depth, value);

                limits.nextItt();
                std::cout << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes << " score cp " << value << " pv " << bestMove << " " << ponder << std::endl;
                this->logFile << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes << " score cp " << value << " pv " << bestMove << " " << ponder << std::endl;
            }
            //Legal check
            board.generate_moves(moves[0]);
            bool found = false;
            for (Move& m: moves[0]){
                if (bestMove == m){
                    found = true;
                    break;
                }
            }
            // Output always legal!
            if (!found){
                std::cout << "info illegal move" << bestMove << "\n";
                bestMove = moves[0][0];
                std::cout << "info selecting new move:" << bestMove << "\n";
            }

            long long unsigned nps = this->nodes;
            if (limits.getElapsed() > 0) {
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
    private:
        /// <summary>
        /// Principle variation search
        /// </summary>
        template<class EvalAgent, bool side>
        int pvs(ClassicBitBoard& board,int depth, int alpha, int beta, Move& bestMove){
            this->nodes++;

            //if time limits is exceeded cuases the parent node to "CUT".
            //return "0" could cause undefined behavior (might be better compared to actual value)
            if (this->limits.exceededTime()) {
                //std::cout << "time exceeded" << std::endl;
                return side?INT_MAX : INT_MIN;
            }

            //probe transposition table
            uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
            bool tablehit = TTtable.contains(key);
            Move firstMove;
            if (tablehit) {
                tableHits++;
                firstMove = TTtable.move(key, this->searchID);
                if (TTtable.depth(key, this->searchID) >= depth) { //Already computed
                    //if (entry.type == TTtype::PV) { //Knuth's Type 1
                    chess::TTtype type = TTtable.type(key, this->searchID);
                    int eval = TTtable.eval(key, this->searchID);
                    bestMove = firstMove;
                    if (type == TTtype::PV) { //Knuth's Type 1
                        return eval;
                    }
                    else if (type == TTtype::CUT) { //Knuth's Type 2 = lower bound
                        if (side) {
                            if (eval >= beta) { return eval; }
                        }else{
                            if (eval <= alpha){ return eval; }
                        }
                    }
                    else if (type == TTtype::ALL) { //Knuth's Type 3 = upper bound
                        if (side){
                            if (eval <= alpha) { return eval; }
                        }else{
                            if (eval >= beta) { return eval; }
                        }
                    }
                    else {
                        std::cout << "Something went wrong????" << std::endl;
                    }
                }
            }

            //check if node is terminal
            board.generate_moves(moves[depth]);
            if (moves[depth].empty()) //No more moves=terminal node
            {
                if (board.isCheck<side>()) {
                    mates++;
                    return side ? -mate_Value - depth : mate_Value + depth; //MATE
                }
                else {
                    draws++;
                    return 0; //DRAW
                }
            }

            //Repetition
            if (this->twoFoldEnabled && board.isTwoFold()) {	// Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }

            //set 'PV move first':
            if (tablehit) {
                board.sort<side>(moves[depth], firstMove);
            }
            else {
                board.sort<side>(moves[depth]);
            }

            int in_alpha = alpha;
            int in_beta = beta;

            //start search
            if (side) {
                int bestValue = INT_MIN;
                int value = INT_MIN;
                bool bSearchPV = true;
                for (Move &m : moves[depth]) {
                    board.makeMove(m);
                    if (depth <= 1) {
                        value = EvalAgent::template eval<!side>(board);
                        nodes++;
                    }
                    else if (bSearchPV) {
                        //First move in node => PV node
                        value = pvs<EvalAgent,!side>(board, depth - 1, alpha, beta, firstMove);
                    } else {
                        value = zwSearch<EvalAgent,!side>(board, depth - 1, alpha,alpha + 1);
                        if (value > alpha && value < beta) {
                            // research
                            research++;
                            value = pvs<EvalAgent,!side>(board, depth - 1, alpha, beta, firstMove);
                        }
                    }
                    board.undoMove();
                    if (value > bestValue) {
                        bestMove.flags = m.flags;
                        bestMove.from = m.from;
                        bestMove.to = m.to;
                        bestMove.IsWhite = m.IsWhite;
                        bestValue = value;
                    }
                    if (value >= beta) {
                        break;//fail-hard beta-cutoff
                        //return beta;
                    }
                    alpha = std::max(alpha, value);
                    bSearchPV = false; //after first node search is set to false => start ZW search;
                }
                if (bestValue > in_alpha && bestValue < in_beta) {
                    TTtable.update(key, this->searchID, TTtype::PV, bestValue, depth, bestMove);
                }
                else if (bestValue >= in_beta) {
                    //note: 'black to move' = minimizing => upper bound = all node
                    TTtable.update(key, this->searchID, TTtype::ALL, bestValue, depth, bestMove);
                }
                else if (bestValue <= in_alpha) {
                    //note: 'black to move' = minimizing => lower bound = cut node
                    TTtable.update(key, this->searchID, TTtype::CUT, bestValue, depth, bestMove);
                }
                return bestValue;
            }
            else{
                int bestValue = INT_MAX;
                int value = INT_MAX;
                bool bSearchPV = true;
                for (Move &m : moves[depth]) {
                    board.makeMove(m);
                    if (depth <= 1) {
                        value = EvalAgent::template eval<!side>(board);
                        nodes++;
                    }
                    else if (bSearchPV) {
                        //First move in node => PV node
                        value =pvs<EvalAgent,!side>(board, depth - 1, alpha, beta,firstMove);
                    } else {
                        value = zwSearch<EvalAgent,!side>(board, depth - 1, beta-1,beta);
                        if (value < beta && value > alpha) {
                            // research
                            value = pvs<EvalAgent,!side>(board, depth - 1, alpha, beta,firstMove);
                        }
                    }
                    board.undoMove();
                    if (value < bestValue) {
                        bestMove.flags = m.flags;
                        bestMove.from = m.from;
                        bestMove.to = m.to;
                        bestMove.IsWhite = m.IsWhite;
                        bestValue = value;
                    }
                    if (value <= alpha) {
                        break; //fail-hard 'beta-cutoff'
                    }
                    beta = std::min(beta, value);
                    bSearchPV = false; //after first node search is set to false => start ZW search;
                }
                if (bestValue > in_alpha && bestValue < in_beta) {
                    TTtable.update(key, this->searchID, TTtype::PV, bestValue, depth, bestMove);
                }
                else if (bestValue >= in_beta) {
                    //note: 'black to move' = minimizing => upper bound = all node
                    TTtable.update(key, this->searchID, TTtype::ALL, bestValue, depth, bestMove);
                }
                else if (bestValue <= in_alpha) {
                    //note: 'black to move' = minimizing => lower bound = cut node
                    TTtable.update(key, this->searchID, TTtype::CUT, bestValue, depth, bestMove);
                }
                return bestValue;
            }
        }
        template<class EvalAgent, bool side>
        int zwSearch(chess::ClassicBitBoard& board, int depth, int alpha, int beta) {
            nodes++;
            //Note: probing table is more expansive compared to gain.
            //Perhapse if "eval" becomes more expensive later on probing might become better!
            //if (depth == 0) {
            //	//if (tablehit) return entry.eval; else
            //	return EvalAgent::template eval<side>(board);
            //}
            //auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->startTime).count();
            //if (elapsed > this->limitTime && this->limitTime > 0) return 0;

            //if time limit is exceeded cause the parent node to "CUT".
            //return "0" could cause undefined behavior (might be better compared to actual value)
            //Only check if white's turn :> half the amount of checks (side = template)
            if (this->limits.exceededTime()) return side ? INT_MAX : INT_MIN;

            //probe transposition table
            uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
            bool tablehit = TTtable.contains(key);
            Move firstMove;
            Move best;
            if (tablehit) {
                tableHits++;
                firstMove = TTtable.move(key, this->searchID);
                if (TTtable.depth(key, this->searchID) >= depth) { //Already computed
                    //if (entry.type == TTtype::PV) { //Knuth's Type 1
                    chess::TTtype type = TTtable.type(key, this->searchID);
                    int eval = TTtable.eval(key, this->searchID);
                    best = firstMove;
                    if (type == TTtype::PV) { //Knuth's Type 1
                        return eval;
                    }
                    else if (type == TTtype::CUT) { //Knuth's Type 2 = lower bound
                        if (side){
                            if (eval >= beta) { return eval; }
                        }else{
                            if( eval <= alpha){ return eval; }
                        }
                    }
                    else if (type == TTtype::ALL) { //Knuth's Type 3 = upper bound
                        if (side){
                            if (eval <= alpha) { return eval; }
                        }else{
                            if (eval >= beta) {return eval; }
                        }
                    }
                    else {
                        std::cout << "Something went wrong????" << std::endl;
                    }
                }
            }
            board.generate_moves(moves[depth]);
            if (moves[depth].empty()) //No more moves=terminal node
            {
                if (board.isCheck<side>()) {
                    mates++;
                    return side ? -mate_Value - depth : mate_Value + depth; //MATE
                }
                else {
                    draws++;
                    return 0; //DRAW
                }
            }
            if (this->twoFoldEnabled && board.isTwoFold()) {	// Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }
            //set 'PV move first':
            if (tablehit) {
                board.sort<side>(moves[depth], firstMove);
            }
            else {
                board.sort<side>(moves[depth]);
            }

            int in_alpha = alpha;
            int in_beta = beta;

            if (side) {
                int bestValue = INT_MIN;
                int value = INT_MIN;
                for (Move& m : moves[depth]) {
                    board.makeMove(m);
                    if (depth <= 1) {
                        value = std::max(value, EvalAgent::template eval<!side>(board));
                        nodes++;
                    }
                    else {
                        value = std::max(value, zwSearch<EvalAgent, !side>(board, depth - 1, alpha, beta));
                    }
                    board.undoMove();
                    //std::cout << board.MovetoSAN(m) << " " << value << std::endl;
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
                    TTtable.update(key, this->searchID, TTtype::CUT, bestValue, depth, best);
                }
                else if (value <= in_alpha) {
                    TTtable.update(key, this->searchID, TTtype::ALL, bestValue, depth, best);
                }
                return bestValue;
            }
            else {
                int bestValue = INT_MAX;
                int value = INT_MAX;
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    if (depth <= 1) {
                        value = std::min(value, EvalAgent::template eval<!side>(board));
                    }
                    else {
                        value = std::min(value, zwSearch<EvalAgent, !side>(board, depth - 1, alpha, beta));
                    }
                    board.undoMove();
                    //std::cout << board.MovetoSAN(m) << " " << value << std::endl;
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
                if (value > in_alpha && value < in_beta) {
                    TTtable.update(key, this->searchID, TTtype::PV, value, depth, best);
                }
                else if (value >= in_beta) {
                    //note: 'black to move' = minimizing => upper bound = all node
                    TTtable.update(key, this->searchID, TTtype::ALL, bestValue, depth, best);
                }
                else if (value <= in_alpha) {
                    //note: 'black to move' = minimizing => lower bound = cut node
                    TTtable.update(key, this->searchID, TTtype::CUT, bestValue, depth, best);
                }
                return bestValue;
            }
        }
    };

    class PVSRazoring{
        Polyglot book;
        std::vector<int> razorMargin = {0,1000,1400,2700};
    public:
        // counters
        uint8_t searchID = 0; //increment every time a new search is started, overflow at the end => not a problem.
        uint64_t nodes;
        uint64_t threefold;
        uint64_t twofold;
        uint64_t mates;
        uint64_t draws;
        uint64_t tableHits;

        //help objects
        std::vector<std::vector<Move>> moves;
        TTentry entry;
        TranspositionTable TTtable = TranspositionTable(1024);
        std::stringstream logFile;

        //UCI object to handle search limits
        UCI::Limits limits;
        bool twoFoldEnabled = true;
        bool threeFoldEnabled = true;

        void setBook(Polyglot& polyglot) {
            this->book = polyglot;
        }
        bool getPonder(chess::ClassicBitBoard& board, Move& ponder) {
            uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
            if (TTtable.contains(key)) {
                Move best = TTtable.move(key, this->searchID);
                board.makeMove(best);
                uint64_t ponder_key = ClassicBitBoard::HashUtil::createHash(board);
                if (TTtable.contains(ponder_key)) {
                    ponder = TTtable.move(ponder_key, this->searchID);
                    board.undoMove();
                    return true;
                }
                board.undoMove();
            }
            return false;
        }

        template<class EvalAgent>
        int search(chess::ClassicBitBoard& board, int depth, Move& bestMove, Move& ponder){
            this->limits.depth= depth;
            return this->search<EvalAgent>(board, bestMove, ponder);
        }
        template<class EvalAgent>
        int search(chess::ClassicBitBoard& board, Move& bestMove, Move& ponder) {
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

            //Initialize return value;
            int value = 0;
            bool is_mate = false;
            int mate_in = 0;

            //first check polyglot table;
            uint64_t key = chess::ClassicBitBoard::HashUtil::createHash(board);
            if (book.getMove(key, bestMove, board)) {
                //std::cout << "info: theory hit" << std::endl;
                ponder = chess::Move();
                //get eval to return this value
                board.makeMove(bestMove);
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
            if (moves[0].empty()) {
                bestMove = Move();
                ponder = Move();
                return 0;
            }
            //1 possible move?
            if (moves[0].size() == 1) {
                bestMove = moves[0][0];
                ponder = chess::Move();
                board.makeMove(bestMove);
                value = board.side ? EvalAgent::eval<true>(board) : EvalAgent::eval<false>(board);
                board.undoMove();
                printIteration(1,board.side?value:-value,bestMove,ponder, false,0);
                return value;
            }
            //mate in 1?
            for (auto& move : moves[0]) {
                board.makeMove(move);
                board.generate_moves(moves[1]);
                if (board.side) {
                    if (moves[1].empty() && board.isCheck<true>()) {
                        //Mate in 1
                        board.undoMove();
                        bestMove = move;
                        ponder = Move();
                        printIteration(1,0,bestMove,ponder, true,1);
                        return -mate_Value;//MATE
                    }
                }
                else {
                    if (moves[1].empty() && board.isCheck<false>()) {
                        //Mate in 1
                        board.undoMove();
                        bestMove = move;
                        ponder = Move();
                        printIteration(1,0,bestMove,ponder, true,1);
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
            bestMove = moves[0][0]; //Already checked this value is >= 1!
            ponder = Move();

            //No early stop conditions => start alpha beta search
            limits.nextItt();
            Move searchMove = bestMove; //no empty move!
            int searchValue;
            int finalDepth = 0;
            for (int depth = 1; !limits.exceeded(depth); depth++) {
                if (depth == moves.size()) moves.resize((size_t)(moves.size() * 1.5) + 1);
                this->searchID++;

                int alpha = INT_MIN;
                int beta = INT_MAX;
                if (board.side)
                    searchValue = pvsRazoring<EvalAgent,true>(board, depth, alpha, beta, searchMove);
                else{
                    searchValue = pvsRazoring<EvalAgent,false>(board, depth, alpha, beta, searchMove);
                }

                if (limits.exceededTime() || searchValue == INT_MAX || searchValue == INT_MIN) {
                    // alpha beta search was terminated prematurely => results not complete
                    break;
                }
                bestMove = searchMove;
                value = board.side?searchValue:-searchValue;
                finalDepth = depth;
                getPonder(board, ponder);
                is_mate = isMate(value);
                mate_in = mateIn(depth, value);

                limits.nextItt();
                printIteration(depth,value,bestMove,ponder, is_mate,mate_in);
                //std::cout << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes << " score cp " << value << " pv " << bestMove << " " << ponder << std::endl;
                //this->logFile << "info depth " << depth << " time " << limits.getElapsed() << " nodes " << this->nodes << " score cp " << value << " pv " << bestMove << " " << ponder << std::endl;
            }
            printFinalValues(finalDepth,value,bestMove,ponder,is_mate,mate_in);
            /*long long unsigned nps = this->nodes;
            if (limits.getElapsed() > 0) {
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
                          << std::endl;*/
            return value;
        }
    private:
        /// <summary>
        /// Principle variation search
        /// </summary>
        template<class EvalAgent, bool side>
        int pvsRazoring(ClassicBitBoard& board,int depth, int alpha, int beta, Move& bestMove){
            this->nodes++;


            //if time limis is exceeded cuase the parent node to "CUT".
            //return "0" could cause undefined behavior (might be better compared to actual value)
            if (this->limits.exceededTime()) {
                return side?INT_MAX : INT_MIN;
            }

            //probe transposition table
            uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
            bool tablehit = TTtable.contains(key);
            Move firstMove;
            if (tablehit) {
                tableHits++;
                firstMove = TTtable.move(key, this->searchID);
                if (TTtable.depth(key, this->searchID) >= depth) { //Already computed
                    //if (entry.type == TTtype::PV) { //Knuth's Type 1
                    chess::TTtype type = TTtable.type(key, this->searchID);
                    int eval = TTtable.eval(key, this->searchID);
                    bestMove = firstMove;
                    if (type == TTtype::PV) { //Knuth's Type 1
                        return eval;
                    }
                    else if (type == TTtype::CUT) { //Knuth's Type 2 = lower bound
                        if (side) {
                            if (eval >= beta) { return eval; }
                        }else{
                            if (eval <= alpha){ return eval; }
                        }
                    }
                    else if (type == TTtype::ALL) { //Knuth's Type 3 = upper bound
                        if (side){
                            if (eval <= alpha) { return eval; }
                        }else{
                            if (eval >= beta) { return eval; }
                        }
                    }
                    else {
                        std::cout << "Something went wrong????" << std::endl;
                    }
                }
            }

            //check if node is terminal
            board.generate_moves(moves[depth]);
            if (moves[depth].empty()) //No more moves=terminal node
            {
                if (board.isCheck<side>()) {
                    mates++;
                    return side ? -mate_Value - depth : mate_Value + depth; //MATE
                }
                else {
                    draws++;
                    return 0; //DRAW
                }
            }

            //Repetition
            if (this->twoFoldEnabled && board.isTwoFold()) {	// Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }

            //set 'PV move first':
            if (tablehit) {
                board.sort<side>(moves[depth], firstMove);
            }
            else {
                board.sort<side>(moves[depth]);
            }

            int in_alpha = alpha;
            int in_beta = beta;

            //start search
            if (side) {
                int bestValue = INT_MIN;
                int value = INT_MIN;
                bool bSearchPV = true;
                for (Move &m : moves[depth]) {
                    board.makeMove(m);
                    if (depth <= 1) {
                        value = EvalAgent::template eval<!side>(board);
                        nodes++;
                    }
                    else if (bSearchPV) {
                        //First move in node => PV node
                        value = pvsRazoring<EvalAgent,!side>(board, depth - 1, alpha, beta, firstMove);
                    }else {
                        value = zwSearch<EvalAgent,!side>(board, depth - 1, alpha,alpha + 1);
                        if (value > alpha && value < beta) {
                            // research
                            value = pvsRazoring<EvalAgent,!side>(board, depth - 1, alpha, beta, firstMove);
                        }
                    }
                    board.undoMove();
                    if (value > bestValue) {
                        bestMove.flags = m.flags;
                        bestMove.from = m.from;
                        bestMove.to = m.to;
                        bestMove.IsWhite = m.IsWhite;
                        bestValue = value;
                    }
                    if (value >= beta) {
                        break;//fail-hard beta-cutoff
                        //return beta;
                    }
                    alpha = std::max(alpha, value);
                    bSearchPV = false; //after first node search is set to false => start ZW search;
                }
                if (bestValue > in_alpha && bestValue < in_beta) {
                    TTtable.update(key, this->searchID, TTtype::PV, bestValue, depth, bestMove);
                }
                else if (bestValue >= in_beta) {
                    //note: 'black to move' = minimizing => upper bound = all node
                    TTtable.update(key, this->searchID, TTtype::ALL, bestValue, depth, bestMove);
                }
                else if (bestValue <= in_alpha) {
                    //note: 'black to move' = minimizing => lower bound = cut node
                    TTtable.update(key, this->searchID, TTtype::CUT, bestValue, depth, bestMove);
                }
                return bestValue;
            }
            else{
                int bestValue = INT_MAX;
                int value = INT_MAX;
                bool bSearchPV = true;
                for (Move &m : moves[depth]) {
                    board.makeMove(m);
                    if (depth <= 1) {
                        value = EvalAgent::template eval<!side>(board);
                        nodes++;
                    }
                    else if (bSearchPV) {
                        //First move in node => PV node
                        value =pvsRazoring<EvalAgent,!side>(board, depth - 1, alpha, beta,firstMove);
                    } else {
                        value = zwSearch<EvalAgent,!side>(board, depth - 1, beta-1,beta);
                        if (value > alpha && value < beta) {
                            // research
                            value = pvsRazoring<EvalAgent,!side>(board, depth - 1, alpha, beta, firstMove);
                        }
                            }
                    board.undoMove();
                    if (value < bestValue) {
                        bestMove.flags = m.flags;
                        bestMove.from = m.from;
                        bestMove.to = m.to;
                        bestMove.IsWhite = m.IsWhite;
                        bestValue = value;
                    }
                    if (value <= alpha) {
                        break; //fail-hard 'beta-cutoff'
                    }
                    beta = std::min(beta, value);
                    bSearchPV = false; //after first node search is set to false => start ZW search;
                }
                if (bestValue > in_alpha && bestValue < in_beta) {
                    TTtable.update(key, this->searchID, TTtype::PV, bestValue, depth, bestMove);
                }
                else if (bestValue >= in_beta) {
                    //note: 'black to move' = minimizing => upper bound = all node
                    TTtable.update(key, this->searchID, TTtype::ALL, bestValue, depth, bestMove);
                }
                else if (bestValue <= in_alpha) {
                    //note: 'black to move' = minimizing => lower bound = cut node
                    TTtable.update(key, this->searchID, TTtype::CUT, bestValue, depth, bestMove);
                }
                return bestValue;
            }
        }
        template<class EvalAgent, bool side>
        int zwSearch(chess::ClassicBitBoard& board, int depth, int alpha, int beta) {
            nodes++;
            bool isRazoring = false;
            if (this->limits.exceededTime()) return side ? INT_MAX : INT_MIN;

            //Note: probing table is more expansive compared to gain.
            //Perhapse if "eval" becomes more expensive later on probing might become better!
            if (depth <= 0) {
            //	//if (tablehit) return entry.eval; else
            	return EvalAgent::template eval<side>(board);
            }
            //auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->startTime).count();
            //if (elapsed > this->limitTime && this->limitTime > 0) return 0;

            //if time limit is exceeded cause the parent node to "CUT".
            //return "0" could cause undefined behavior (might be better compared to actual value)

            //probe transposition table
            uint64_t key = ClassicBitBoard::HashUtil::createHash(board);
            bool tablehit = TTtable.contains(key);
            Move firstMove;
            Move best;
            if (tablehit) {
                tableHits++;
                firstMove = TTtable.move(key, this->searchID);
                if (TTtable.depth(key, this->searchID) >= depth) { //Already computed
                    //if (entry.type == TTtype::PV) { //Knuth's Type 1
                    chess::TTtype type = TTtable.type(key, this->searchID);
                    int eval = TTtable.eval(key, this->searchID);
                    best = firstMove;
                    if (type == TTtype::PV) { //Knuth's Type 1
                        return eval;
                    }
                    else if (type == TTtype::CUT) { //Knuth's Type 2 = lower bound
                        if (side){
                            if (eval >= beta) { return eval; }
                        }else{
                            if( eval <= alpha){ return eval; }
                        }
                    }
                    else if (type == TTtype::ALL) { //Knuth's Type 3 = upper bound
                        if (side){
                            if (eval <= alpha) { return eval; }
                        }else{
                            if (eval >= beta) {return eval; }
                        }
                    }
                    else {
                        std::cout << "Something went wrong????" << std::endl;
                    }
                }
            }
            board.generate_moves(moves[depth]);
            if (moves[depth].empty()) //No more moves=terminal node
            {
                if (board.isCheck<side>()) {
                    mates++;
                    return side ? -mate_Value - depth : mate_Value + depth; //MATE
                }
                else {
                    draws++;
                    return 0; //DRAW
                }
            }
            if (this->twoFoldEnabled && board.isTwoFold()) {	// Twofold repetition => not making 'progress' => count as a draw
                twofold++;
                return draw_value;
            }
            else if (this->threeFoldEnabled && board.isThreeFold()) {
                threefold++;
                return draw_value;
            }
            //set 'PV move first':
            if (tablehit) {
                board.sort<side>(moves[depth], firstMove);
            }
            else {
                board.sort<side>(moves[depth]);
            }

            int in_alpha = alpha;
            int in_beta = beta;

            if (side) {//white, maximising
                int bestValue = INT_MIN;
                int value = INT_MIN;
                for (Move& m : moves[depth]) {
                    board.makeMove(m);
                    if (depth <= 1) {
                        value = std::max(value, EvalAgent::template eval<!side>(board));
                        nodes++;
                    }
                    else {
                        if(!isRazoring){
                            if(depth > 3){
                                value = std::max(value, zwSearch<EvalAgent, !side>(board, depth - 1, alpha, beta));
                            }
                            else{
                                if(m!=firstMove&&!(m.to&board.Enemy<side>())){
                                    //wat is de value op deze diepte?
                                    //value = std::max(value, EvalAgent::template eval<!side>(board));
                                    if(value + razorMargin[depth] < alpha){
                                        isRazoring = true;
                                        value = std::max(value, zwSearch<EvalAgent, !side>(board, depth - 2, alpha, beta));
                                    }
                                }
                                else{
                                    value = std::max(value, zwSearch<EvalAgent, !side>(board, depth - 1, alpha, beta));
                                }
                            }
                        }
                        else{
                            value = std::max(value, zwSearch<EvalAgent, !side>(board, depth - 2, alpha, beta));
                        }
                    }
                    board.undoMove();
                    //std::cout << board.MovetoSAN(m) << " " << value << std::endl;
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
                    TTtable.update(key, this->searchID, TTtype::CUT, bestValue, depth, best);
                }
                else if (value <= in_alpha) {
                    TTtable.update(key, this->searchID, TTtype::ALL, bestValue, depth, best);
                }
                return bestValue;
            }
            else { //black, minimizing
                int bestValue = INT_MAX;
                int value = INT_MAX;
                for (Move& m : moves[depth]) {
                    board.makeMove(m); nodes++;
                    if (depth <= 1) {
                        value = std::min(value, EvalAgent::template eval<!side>(board));
                    }
                    else {
                        if(!isRazoring){
                            if(depth > 3){
                                value = std::min(value, zwSearch<EvalAgent, !side>(board, depth - 1, alpha, beta));
                            }
                            else{
                                if(m!=firstMove&&!(m.to&board.Enemy<side>())){
                                    //wat is de value op deze diepte?
                                    //value = std::max(value, EvalAgent::template eval<!side>(board));
                                    if(value - razorMargin[depth] > beta){
                                        isRazoring = true;
                                        value = std::min(value, zwSearch<EvalAgent, !side>(board, depth - 2, alpha, beta));
                                    }
                                    else{
                                        value = std::min(value, zwSearch<EvalAgent, !side>(board, depth - 1, alpha, beta));
                                    }
                                }
                                else{
                                    value = std::min(value, zwSearch<EvalAgent, !side>(board, depth - 1, alpha, beta));
                                }
                            }
                        }
                        else{
                            value = std::min(value, zwSearch<EvalAgent, !side>(board, depth - 2, alpha, beta));
                        }
                    }
                    board.undoMove();
                    //std::cout << board.MovetoSAN(m) << " " << value << std::endl;
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
                if (value > in_alpha && value < in_beta) {
                    TTtable.update(key, this->searchID, TTtype::PV, value, depth, best);
                }
                else if (value >= in_beta) {
                    //note: 'black to move' = minimizing => upper bound = all node
                    TTtable.update(key, this->searchID, TTtype::ALL, bestValue, depth, best);
                }
                else if (value <= in_alpha) {
                    //note: 'black to move' = minimizing => lower bound = cut node
                    TTtable.update(key, this->searchID, TTtype::CUT, bestValue, depth, best);
                }
                return bestValue;
            }
        }

        void printIteration(int depth, int value, Move& bestMove, Move& ponder, int is_mate=false, int mate_in=0);
        void printFinalValues(int depth, int value, Move& bestMove, Move& ponder, int is_mate=false, int mate_in=0);
    };
}

