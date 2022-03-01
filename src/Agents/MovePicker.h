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
    winningCapture = 1,
    equalCapture = 2,
    killerFirst = 3,
    killerSecond = 4,
    other = 6,
    capture
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
                state = winningCapture;
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
            case winningCapture:{
                for (auto it = moveBuffer->begin(); it != moveBuffer->end(); it++) {
                    if (board->isWinningCapture<IsWhite>(*it)){
                        move = *it;
                        std::swap(*it, moveBuffer->back());
                        moveBuffer->pop_back();
                        return true;
                    }
                }
                state = equalCapture;
                [[fallthrough]];
            }
            case equalCapture:{
                for (auto it = moveBuffer->begin(); it != moveBuffer->end(); it++) {
                    if (board->isEqualCapture<IsWhite>(*it)){
                        move = *it;
                        std::swap(*it, moveBuffer->back());
                        moveBuffer->pop_back();
                        return true;
                    }
                }
                state = killerFirst;
                [[fallthrough]];
            }
            case killerFirst: {
                state = killerSecond; //already change state => only 1 possible hit killer moves
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
                state = other; //already change state => only 1 possible hit for killer moves
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
            /*case capture: {
                for (auto it = moveBuffer->begin(); it != moveBuffer->end(); it++) {
                    if (board->isCapture<IsWhite>(*it)) {
                        move = *it;
                        std::swap(*it, moveBuffer->back());
                        moveBuffer->pop_back();
                        return true;
                    }
                }
                state = killerFirst;
                [[fallthrough]];
            }*/
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
    pickerState getState(){
        return this->state;
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
