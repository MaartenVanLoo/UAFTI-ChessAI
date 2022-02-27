//
// Created by Maarten Van Loo on 27/02/2022.
//

#ifndef CMAKELISTS_TXT_MOVEPICKER_H
#define CMAKELISTS_TXT_MOVEPICKER_H
#include "TranspositionTable.h"
#include "../chess/ClassicBitBoard.h"

using namespace chess;

enum pickerState{
    transposition = 0,
    killerFirst = 1,
    killerSecond = 2,
    capture = 3,
    other = 4
};
template <bool IsWhite>
class MovePicker {
public:
    MovePicker()=delete;
    MovePicker(ClassicBitBoard& board, std::vector<Move>& moveBuffer,
               std::pair<Move,Move>& killers, bool tableHit, Move& transpositionMove){
        this->board = &board;
        this->moveBuffer = &moveBuffer;
        this->killers = &killers;
        this->tableHit =tableHit;
        this->transpositionMove = &transpositionMove;
        this->state =transposition;
    };

    bool nextMove(Move& move){
        switch(state){
            case transposition: {
                state = killerFirst;
                if (tableHit) {
                    for (auto it = moveBuffer->begin(); it != moveBuffer->end(); it++) {
                        if (*transpositionMove == *it) {
                            std::swap(*it, moveBuffer->back());
                            moveBuffer->pop_back();
                            move = *transpositionMove;
                            return true;
                        }
                    }
                }
                [[fallthrough]];
            }
            case killerFirst: {
                state = killerSecond;
                for (auto it = moveBuffer->begin(); it != moveBuffer->end(); it++) {
                    if (killers->first == *it) {
                        std::swap(*it, moveBuffer->back());
                        moveBuffer->pop_back();
                        move = killers->first;
                        return true;
                    }
                }
                [[fallthrough]];
            }
            case killerSecond: {
                state = capture;
                for (auto it = moveBuffer->begin(); it != moveBuffer->end(); it++) {
                    if (killers->second == *it) {
                        std::swap(*it, moveBuffer->back());
                        moveBuffer->pop_back();
                        move = killers->second;
                        return true;
                    }
                }
                [[fallthrough]];
            }
            case capture: {
                for (auto it = moveBuffer->begin(); it != moveBuffer->end(); it++) {
                    if (it->to & this->board->template Enemy<IsWhite>()) {
                        move = *it;
                        std::swap(*it, moveBuffer->back());
                        moveBuffer->pop_back();
                        return true;
                    }
                }
                state = other;
                [[fallthrough]];
            }
            case other: {
                if (moveBuffer->empty()) return false;
                else {
                    move = moveBuffer->back();
                    moveBuffer->pop_back();
                    return true;
                }
            }
        }
        return false;
    }
private:
    ClassicBitBoard* board;
    pickerState state = transposition;
    std::pair<Move,Move>* killers = nullptr;
    std::vector<Move>* moveBuffer;

    bool tableHit = false;
    Move* transpositionMove = nullptr;
};


#endif //CMAKELISTS_TXT_MOVEPICKER_H
