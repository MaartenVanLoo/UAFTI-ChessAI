//
// Created by maart on 13/12/2021.
//

#ifndef UAFTI_CHESSAI_CHESSAIMODEL_H
#define UAFTI_CHESSAI_CHESSAIMODEL_H
#include <torch/torch.h>
#include "../chess/ClassicBitBoard.h"

class ChessAIModel {
public:
    static torch::Tensor boardToTensor(chess::ClassicBitBoard& board) {
        torch::Tensor tensor = torch::zeros(784);
        uint64_t blackPawns = board.BPawn;
        uint64_t blackRooks = board.BRook;
        uint64_t blackBishops = board.BBishop;
        uint64_t blackKnights = board.BKnight;
        uint64_t blackQueens = board.BQueen;
        uint64_t blackKings = board.BKing;
        uint64_t whitePawns = board.WPawn;
        uint64_t whiteRooks = board.WRook;
        uint64_t whiteBishops = board.WBishop;
        uint64_t whiteKnights = board.WKnight;
        uint64_t whiteQueens = board.WQueen;
        uint64_t whiteKings = board.WKing;

        uint64_t sq;
        // blackpawns
        Bitloop(blackPawns){
            sq = SquareOf(blackPawns);
            tensor[0+sq] = 1;
        }
        // blackRooks
        Bitloop(blackRooks) {
            sq = SquareOf(blackRooks);
            tensor[64 + sq] = 1;
        }
        // blackBishops
        Bitloop(blackBishops){
            sq = SquareOf(blackBishops);
            tensor[128+sq] = 1;
        }
        // blackKnights
        Bitloop(blackKnights){
            sq = SquareOf(blackKnights);
            tensor[192+sq] = 1;
        }
        // blackQueens
        Bitloop(blackQueens){
            sq = SquareOf(blackQueens);
            tensor[256+sq] = 1;
        }
        // blackKings
        Bitloop(blackKings){
            sq = SquareOf(blackKings);
            tensor[320+sq] = 1;
        }
        // whitePawns
        Bitloop(whitePawns){
            sq = SquareOf(whitePawns);
            tensor[384+sq] = 1;
        }
        // whiteRooks
        Bitloop(whiteRooks){
            sq = SquareOf(whiteRooks);
            tensor[448+sq] = 1;
        }
        // whiteBishops
        Bitloop(whiteBishops){
            sq = SquareOf(whiteBishops);
            tensor[512+sq] = 1;
        }
        // whiteKnights
        Bitloop(whiteKnights){
            sq = SquareOf(whiteKnights);
            tensor[576+sq] = 1;
        }
        // whiteQueens
        Bitloop(whiteQueens){
            sq = SquareOf(whiteQueens);
            tensor[640+sq] = 1;
        }
        // whiteKings
        Bitloop(whiteKings){
            sq = SquareOf(whiteKings);
            tensor[704+sq] = 1;
        }

        // castling
        if (board.state.BCastleL) tensor[768] = 1;
        if (board.state.BCastleR) tensor[769] = 1;
        if (board.state.WCastleL) tensor[770] = 1;
        if (board.state.WCastleR) tensor[771] = 1;

        // ep
        if (board.state.hasEP){
            int file = (int)(SquareOf(board.EnPassantTarget) % 8);
            tensor[772 + file] = 1; // h file = 0; a file = 7;
        }
        tensor[780] = (int)board.halfmoves;
        if (board.side){
            tensor[782] = 0;
            tensor[783] = 1;
        }else{
            tensor[782] = 1;
            tensor[783] = 0;
        }
        return tensor;
    }
};


#endif //UAFTI_CHESSAI_CHESSAIMODEL_H
