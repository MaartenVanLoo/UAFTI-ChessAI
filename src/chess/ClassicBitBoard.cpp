#include "ClassicBitBoard.h"
#include <iostream>
#include <regex>
#include <sstream>
namespace chess {
    
    /***************************************\

            CLASS CLASSIC BIT BOARD

    \***************************************/
    #pragma region ClassicBitBoard
    ClassicBitBoard::ClassicBitBoard()
    {
        parse_fen(chess::ClassicBitBoard::startpos);
        White = WPawn | WBishop | WKnight | WRook | WKing | WQueen;
        Black = BPawn | BBishop | BKnight | BRook | BKing | BQueen;
        Occ = White | Black;
    }

    ClassicBitBoard::ClassicBitBoard(std::string fen)
    {
        parse_fen(fen);
        White = WPawn | WBishop | WKnight | WRook | WKing | WQueen;
        Black = BPawn | BBishop | BKnight | BRook | BKing | BQueen;
        Occ = White | Black;
    }

    ClassicBitBoard::ClassicBitBoard(std::string fen, int historySize)
    {
        parse_fen(fen);
        White = WPawn | WBishop | WKnight | WRook | WKing | WQueen;
        Black = BPawn | BBishop | BKnight | BRook | BKing | BQueen;
        Occ = White | Black;
        this->boardHistory = MoveHistory(historySize);
    }

    inline void ClassicBitBoard::makeMove(const Move& move)
    {
        /*if (move.IsWhite) {
            if (King<false>() == move.to) {
                //Moving the place of the king = capturing king = mate!, from this percpective this doesn't cause an error and should be allowed
                //This can happen when loading a fen where the king is attacked by the same color that is allowed to move. 
                std::cout << "ERROR***************************************************ERROR";
            }
        }
        else {
            if (King<true>() == move.to) {
                //Moving the place of the king = capturing king = mate!, from this percpective this doesn't cause an error and should be allowed
                //This can happen when loading a fen where the king is attacked by the same color that is allowed to move.
                std::cout << "ERROR***************************************************ERROR";
            }
        }*/
        boardHistory.push(History(*this));
        switch (getMoveType(move.flags))
        {
        case 0: //normal
            move.IsWhite ? makeMove_Normal<true>(move) : makeMove_Normal<false>(move);
            break;
        case 1: //Promote
            move.IsWhite ? makeMove_Promote<true>(move) : makeMove_Promote<false>(move);
            break;
        case 2: //EP
            move.IsWhite ? makeMove_EP<true>(move) : makeMove_EP<false>(move);
            break;
        case 4: //Castle Left
            move.IsWhite ? makeMove_Castle<true,true>(move) : makeMove_Castle<false,true>(move);
            break;
        case 8: //Castle Right
            move.IsWhite ? makeMove_Castle<true,false>(move) : makeMove_Castle<false,false>(move);
            break;
        default:
            break;
        }
        White = WPawn | WBishop | WKnight | WRook | WKing | WQueen;
        Black = BPawn | BBishop | BKnight | BRook | BKing | BQueen;
        Occ = White | Black;
        side = !side;
        if (side) fullmoves += 1; //black has just made a move, now white can play again.
    }

    inline bool ClassicBitBoard::undoMove()
    {
        History newBoard = boardHistory.pop();
        if (this->side) fullmoves -= 1; //white move is reset => also reduce fullmoves;.
        this->BPawn = newBoard.BPawn;
        this->BKnight = newBoard.BKnight;
        this->BBishop = newBoard.BBishop;
        this->BRook = newBoard.BRook;
        this->BQueen = newBoard.BQueen;
        this->BKing = newBoard.BKing;
        this->WPawn = newBoard.WPawn;
        this->WKnight = newBoard.WKnight;
        this->WBishop = newBoard.WBishop;
        this->WRook = newBoard.WRook;
        this->WQueen = newBoard.WQueen;
        this->WKing = newBoard.WKing;

        this->Black = newBoard.Black;
        this->White = newBoard.White;
        this->Occ = newBoard.Occ;
        this->EnPassantTarget = newBoard.EnPassantTarget;
        this->rookPin = newBoard.rookPin;
        this->bishopPin = newBoard.bishopPin;
        this->halfmoves = newBoard.halfMoves;
        this->state.hasEP = newBoard.hasEP;
        this->state.BCastleL = newBoard.BCastleL;
        this->state.BCastleR = newBoard.BCastleR;
        this->state.WCastleL = newBoard.WCastleL;
        this->state.WCastleR = newBoard.WCastleR;
        this->side = !this->side;
        return true;
    }

    void ClassicBitBoard::generate_capture_moves(std::vector<Move>& moves){
        moves.reserve(10);
        moves.clear();
        side ?_generate_capture_moves<true>(moves):_generate_capture_moves<false>(moves);
    }
    void ClassicBitBoard::generate_moves(std::vector<Move>& moves)
    {
        moves.reserve(35);
        moves.clear();
        side ? _generate_moves<true>(moves) : _generate_moves<false>(moves);
        //moves.shrink_to_fit();
    }

    std::ostream& operator<<(std::ostream& os, const ClassicBitBoard& brd)
    {
        os << "\n +---+---+---+---+---+---+---+---+\n ";
        for (int row = 7; row >= 0; --row) {
            for (int col = 7; col >= 0; --col) {
                bit sq = 1ull << (row * 8 + col);
                if (brd.Occ & sq) { // a piece is located at this position
                    if (brd.WPawn & sq) os << "| P ";                    
                    if (brd.BPawn & sq) os << "| p ";

                    if (brd.BKnight & sq) { os << "| n "; }
                    if (brd.BBishop & sq) os << "| b ";
                    if (brd.BRook & sq) os << "| r ";
                    if (brd.BQueen & sq) os << "| q ";
                    if (brd.BKing & sq) os << "| k ";

                    if (brd.WKnight & sq) os << "| N ";
                    if (brd.WBishop & sq) os << "| B ";
                    if (brd.WRook & sq) os << "| R ";
                    if (brd.WQueen & sq) os << "| Q ";
                    if (brd.WKing & sq) os << "| K ";
                }
                else {
                    os << "|   ";
                }
            }
            os << "| " << std::to_string(row + 1) << "\n +---+---+---+---+---+---+---+---+\n ";
        }
        os << "  a   b   c   d   e   f   g   h\n";
        return os;
    }
    std::string ClassicBitBoard::toASCII()
    {
        std::string ascii = "\n +---+---+---+---+---+---+---+---+\n ";
        for (int row = 7; row >= 0; --row) {
            for (int col = 7; col >= 0; --col) {
                bit sq = 1ull << (row * 8 + col);
                if (Occ & sq) { // a piece is located at this position
                    if (this->WPawn & sq) ascii += "| P ";
                    if (this->BPawn & sq) ascii += "| p ";

                    if (this->BKnight & sq) ascii += "| n ";
                    if (this->BBishop & sq) ascii += "| b ";
                    if (this->BRook & sq) ascii += "| r ";
                    if (this->BQueen & sq) ascii += "| q ";
                    if (this->BKing & sq) ascii += "| k ";

                    if (this->WKnight & sq) ascii += "| N ";
                    if (this->WBishop & sq) ascii += "| B ";
                    if (this->WRook & sq) ascii += "| R ";
                    if (this->WQueen & sq) ascii += "| Q ";
                    if (this->WKing & sq) ascii += "| K ";
                }
                else {
                    ascii += "|   ";
                }
            }
            ascii += "| " + std::to_string(row + 1) + "\n +---+---+---+---+---+---+---+---+\n ";
        }
        ascii += "  a   b   c   d   e   f   g   h\n";
        return ascii;
    }

    void ClassicBitBoard::parse_fen(std::string FEN)
    {
        BPawn = fenToBmp(FEN, 'p');
        BKnight = fenToBmp(FEN, 'n');
        BBishop = fenToBmp(FEN, 'b');
        BRook = fenToBmp(FEN, 'r');
        BQueen = fenToBmp(FEN, 'q');
        BKing = fenToBmp(FEN, 'k');
        WPawn = fenToBmp(FEN, 'P');
        WKnight = fenToBmp(FEN, 'N');
        WBishop = fenToBmp(FEN, 'B');
        WRook = fenToBmp(FEN, 'R');
        WQueen = fenToBmp(FEN, 'Q');
        WKing = fenToBmp(FEN, 'K');
        fenToInfo(FEN);
    }
    map ClassicBitBoard::fenToBmp(std::string& FEN, char piece) {
         
        uint64_t i = 0;
        char c{};
        int Field = 63;

        uint64_t result = 0;
        while ((c = FEN[i++]) != ' ')
        {
            uint64_t P = 1ull << Field;
            switch (c) {
            case '/': Field += 1; break;
            case '1': break;
            case '2': Field -= 1; break;
            case '3': Field -= 2; break;
            case '4': Field -= 3; break;
            case '5': Field -= 4; break;
            case '6': Field -= 5; break;
            case '7': Field -= 6; break;
            case '8': Field -= 7; break;
            default:
                if (c == piece) result |= P; //constexpr parsing happens here
            }
            Field--;
        }
        return result;
    }
    void ClassicBitBoard::fenToInfo(std::string& FEN) {
        uint64_t i = 0;
        char c{};

        while ((c = FEN[i++]) != ' ') {}
        char wb = FEN[i++];

        //White
        if (wb == 'w') side =  true;
        else side = false;
        
        i++;
        //Castling
        state.WCastleR = false;
        state.WCastleL = false;
        state.BCastleR = false;
        state.BCastleL = false;
        while ((c = FEN[i++]) != ' ')
        {
            if (c == 'K') state.WCastleR = true;
            if (c == 'Q') state.WCastleL = true;
            if (c == 'k') state.BCastleR =  true;
            if (c == 'q') state.BCastleL = true;            
        }
        //En Passant
        char EorMinus = FEN[i++];
        state.hasEP = false;
        this->EnPassantTarget = 0;
        if (EorMinus != '-') {
            const square sq = uint64_t(((wb == 'w')?32:24) + ('h' - EorMinus));
            this->EnPassantTarget = 1ull << sq;
            this->state.hasEP = true;
            i++; //skip rank vanlue
        }
        i++;

        //Halfmove clock
        this->halfmoves = 0;
        while ((c = FEN[i++]) != ' ') {
            halfmoves *= 10;
            halfmoves += uint64_t(c - '0');
        }
        //Move clock
        this->fullmoves = 0;
        while ((c = FEN[i++]) != '\0') {
            fullmoves *= 10;
            fullmoves += uint64_t(c - '0');
        }

    }

    template <bool IsWhite>
    __forceinline void ClassicBitBoard::makeMove_Normal(const Move& move)
    {
        uint8_t piece = getMoveOwnPiece(move.flags);
        const uint64_t mask = move.from | move.to;
        const uint64_t rem = ~(move.to & Enemy<IsWhite>());
        if (~rem) { halfmoves = 0; }
        else { halfmoves += 1; }
        // if a piece is placed at the 'to' location => to & white will set this bit, by negating only this bit will be cleared 
        if constexpr (IsWhite) {
            //caputure piece 
            BPawn &= rem;
            BKnight &= rem;
            BBishop &= rem;
            BRook &= rem;
            BQueen &= rem;
            //move own piece
            switch (piece)
            {
            case BoardPiece::bp_Pawn:
                WPawn ^= mask;
                if (move.from << 16 == move.to) { updateState<IsWhite, true>(move); }
                else { updateState<IsWhite, false>(move); }
                halfmoves = 0;
                break;
            case BoardPiece::bp_Bishop:
                WBishop ^= mask;
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after moving a bishop;
                //state.hasEP = false;    
                //EnPassantTarget = 0;
                break;
            case BoardPiece::bp_Knight:
                WKnight ^= mask;
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after moving a knight;
                //todo: check if needed!
                //state.hasEP = false;    
                //EnPassantTarget = 0;
                break;
            case BoardPiece::bp_Rook:
                WRook ^= mask;
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after moving a rook;
                break;
            case BoardPiece::bp_Queen:
                WQueen ^= mask;
                updateState<IsWhite, false>(move);//Not possible to have an epsquare after  moving a queen;
                //todo: check if needed!
                //state.hasEP = false;    
                //EnPassantTarget = 0;
                break;
            case BoardPiece::bp_King:
                WKing ^= mask;
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after moving a king;
                break;
            default:
                break;
            }
        }
        else {
            //caputure piece 
            WPawn &= rem;
            WKnight &= rem;
            WBishop &= rem;
            WRook &= rem;
            WQueen &= rem;
            //move own piece
            switch (piece)
            {
            case BoardPiece::bp_Pawn:
                BPawn ^= mask;
                if (move.from >> 16 == move.to) { updateState<IsWhite, true>(move); }
                else { updateState<IsWhite, false>(move); }
                halfmoves = 0;
                break;
            case BoardPiece::bp_Bishop:
                BBishop ^= mask;
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after moving a bishop;
                //todo: check if needed!
                //state.hasEP = false;    
                //EnPassantTarget = 0;
                break;
            case BoardPiece::bp_Knight:
                BKnight ^= mask;
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after moving a knight;
                //todo: check if needed!
                //state.hasEP = false;    
                //EnPassantTarget = 0;
                break;
            case BoardPiece::bp_Rook:
                BRook ^= mask;
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after moving a rook;
                break;
            case BoardPiece::bp_Queen:
                BQueen ^= mask;      
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after  moving a queen;
                //todo: check if needed!
                //state.hasEP = false;    
                //EnPassantTarget = 0;
                break;
            case BoardPiece::bp_King:
                BKing ^= mask;
                updateState<IsWhite, false>(move); //Not possible to have an epsquare after moving a king;
                break;
            default:
                break;
            }
        }
        
    }
    
    template <bool IsWhite>
    __forceinline void ClassicBitBoard::makeMove_Promote(const Move& move)
    {
        uint8_t promotedPiece = getMovePromote(move.flags);
        const uint64_t rem = ~(move.to & Enemy<IsWhite>());
        halfmoves = 0; //moving a pawn reset halfmove clock
        if constexpr (IsWhite) {
            //caputure piece 
            BPawn &= rem;
            BKnight &= rem;
            BBishop &= rem;
            BRook &= rem;
            BQueen &= rem;
            //move own piece
            WPawn ^= move.from;
            switch(promotedPiece){
            case BoardPiece::bp_Bishop:
                WBishop ^= move.to;
                break;
            case BoardPiece::bp_Knight:
                WKnight ^= move.to;
                break;
            case BoardPiece::bp_Rook:
                WRook ^= move.to;
                break;
            case BoardPiece::bp_Queen:
                WQueen ^= move.to;
                break;
            default:
                break;
            }
        }
        else {
            //caputure piece 
            WPawn &= rem;
            WKnight &= rem;
            WBishop &= rem;
            WRook &= rem;
            WQueen &= rem;
            //move own piece
            BPawn ^= move.from;
            switch (promotedPiece){
            case BoardPiece::bp_Bishop:
                BBishop ^= move.to;
                break;
            case BoardPiece::bp_Knight:
                BKnight ^= move.to;
                break;
            case BoardPiece::bp_Rook:
                BRook ^= move.to;
                break;
            case BoardPiece::bp_Queen:
                BQueen ^= move.to;
                break;
            default:
                break;
            }
        }
        updateState<IsWhite, false>(move);  //Not possible to have an epsquare after promotion;
        //todo: check if needed!
        //state.hasEP = false;   
        //EnPassantTarget = 0;
    }

    template<bool IsWhite>
    __forceinline void ClassicBitBoard::makeMove_EP(const Move& move)
    {
        uint8_t piece = getMoveOwnPiece(move.flags);// No need to reqest target piece = always pawn!
        halfmoves = 0; //moving a pawn reset halfmove clock
        //move
        if (IsWhite) {
            WPawn ^= (move.from | move.to);
            BPawn ^= move.to >> 8;
        }
        else {
            WPawn ^= move.to << 8;
            BPawn ^= (move.from | move.to);
        }
        
        //todo: check if needed!
        state.hasEP = false;    //Not possible to have an epsquare after taking a pawn with enpassant;
        EnPassantTarget = 0;

    }

    template <bool IsWhite, bool IsLeft>
    void ClassicBitBoard::makeMove_Castle(const Move& move)
    {
        uint8_t piece = getMoveOwnPiece(move.flags);
        map rookmask = 0;
        halfmoves += 1; //castling always increases halfMoves
        if constexpr (IsLeft) {
            rookmask = Castle_RookswitchL<IsWhite>();
        }
        else {
            rookmask = Castle_RookswitchR<IsWhite>();
        }
        if constexpr (IsWhite) {
            WKing ^= (move.from | move.to);
            WRook ^= rookmask;
            state.WCastleL = false; //Revoke castling rights after castling
            state.WCastleR = false; //Revoke castling rights after castling

             //todo: check if needed!
            state.hasEP = false;    //Not possible to have an epsquare after castling;
            EnPassantTarget = 0;
        }
        else {
            BKing ^= (move.from | move.to);
            BRook ^= rookmask;
            state.BCastleL = false; //Revoke castling rights after castling
            state.BCastleR = false; //Revoke castling rights after castling

             //todo: check if needed!
            state.hasEP = false;    //Not possible to have an epsquare after castling;
            EnPassantTarget = 0;
        }
    }
    
    template<bool IsWhite>
    inline void ClassicBitBoard::_generate_capture_moves(std::vector<Move>& moves){
        constexpr bool white = IsWhite;
        constexpr bool enemy = !IsWhite;
        if (!checkStatusValid) { initCheckStatus<white>(); }

        map checkmask = checkStatus;
        map kingban = lookup::King(SquareOf(King<enemy>()));
        map kingatk = Refresh<IsWhite>(kingban, checkmask);

        if (checkmask == 0xffffffffffffffffull) {
            __generate_capture_moves<white, false>(kingatk, kingban, checkmask, moves);
        }
        else if (checkmask != 0) {
            __generate_capture_moves<white, true>(kingatk, kingban, checkmask, moves);
        }
        else {
            Bitloop(kingatk) {
                const square sq = SquareOf(kingatk);
                if ((1ull << sq) & Enemy<IsWhite>()){
                    //enemy at'to' square => capture move
                    moves.emplace_back(IsWhite, King<IsWhite>(), 1ull << sq, encode_flags(0b0000, BoardPiece::bp_King, 0));
                }
            }
        }
    }

    template<bool IsWhite>
    inline void ClassicBitBoard::_generate_moves(std::vector<Move>& moves)
    {
        constexpr bool white = IsWhite;
        constexpr bool enemy = !IsWhite;
        if (!checkStatusValid) { initCheckStatus<white>(); }

        map checkmask = checkStatus;
        map kingban = lookup::King(SquareOf(King<enemy>()));
        map kingatk = Refresh<IsWhite>(kingban, checkmask);

        if (checkmask == 0xffffffffffffffffull) {
            __generate_moves<white, false>(kingatk, kingban, checkmask, moves);
        }
        else if (checkmask != 0) {
            __generate_moves<white, true>(kingatk, kingban, checkmask, moves);
        }
        else {
            Bitloop(kingatk) {
                const square sq = SquareOf(kingatk);
                //Callback_Move::template Kingmove<status, depth>(brd, King<status.WhiteMove>(brd), 1ull << sq);
                moves.emplace_back(IsWhite, King<IsWhite>(), 1ull << sq, encode_flags(0b0000, BoardPiece::bp_King, 0));
            }
        }
    }

    std::string chess::ClassicBitBoard::getFen()
    {
        std::string fen;
        fen.reserve(80);
        
        int field;
        //loop over all squares:
        for (int row = 7; row >= 0; --row) {
            field = 0;
            for (int col = 7; col >= 0; --col) {
                bit sq = 1ull << (row * 8 + col);                
                if (this->Occ & sq) {
                    if (field != 0) {
                        fen += std::to_string(field);
                        field = 0;
                    }
                    //piece on sq, find which piece:
                    if (this->WPawn & sq) fen += "P";
                    if (this->BPawn & sq) fen += "p";

                    if (this->BKnight & sq) fen += "n";
                    if (this->BBishop & sq) fen += "b";
                    if (this->BRook & sq) fen += "r";
                    if (this->BQueen & sq) fen += "q";
                    if (this->BKing & sq) fen += "k";

                    if (this->WKnight & sq) fen += "N";
                    if (this->WBishop & sq) fen += "B";
                    if (this->WRook & sq) fen += "R";
                    if (this->WQueen & sq) fen += "Q";
                    if (this->WKing & sq) fen += "K";
                }
                else {
                    field += 1;
                }                
            }
            if (field != 0) fen += std::to_string(field);            
            if (row != 0) fen += '/';
        }

        //add current side to move:
        fen += this->side ? " w" : " b";
        
        // add castling rules:
        fen += " ";
        if (this->state.BCastleL || this->state.BCastleR ||
            this->state.WCastleL || this->state.WCastleR) {
            if (this->state.WCastleR) fen += "K";
            if (this->state.WCastleL) fen += "Q";
            if (this->state.BCastleR) fen += "k";
            if (this->state.BCastleL) fen += "q";
        }
        else {
            fen += "-";
        }

        // epsquare
        if (this->state.hasEP) {
            bit target = this->side ? (this->EnPassantTarget << 8) : (this->EnPassantTarget >> 8);
            fen += " " + algebraic(target);
        }
        else {
            fen += " -";
        }

        //halfMoves
        fen += " " + std::to_string(this->halfmoves);
        
        //fullmoves
        fen += " " + std::to_string(this->fullmoves);
        fen.shrink_to_fit();
        return fen;
    }

    bool chess::ClassicBitBoard::LAN2Move(const std::string& lan, Move& move) {
        //split lan => always 4 characters present
        bit from = 1ull << (('h' - lan[0]) + (lan[1] - '1') * 8);
        bit to = 1ull << (('h' - lan[2]) + (lan[3] - '1') * 8);

        //get flags
        BoardPiece promotion = BoardPiece::bp_Pawn;
        if (lan.size() == 5) {
            //promotion piece
            promotion = static_cast<BoardPiece>(std::string(" nbrq").find(lan[4]));
        }
        //source piece:
        BoardPiece piece;
        if (this->side) {
            if (this->WPawn & from) piece = BoardPiece::bp_Pawn;
            else if (this->WKnight & from) piece = BoardPiece::bp_Knight;
            else if (this->WBishop & from) piece = BoardPiece::bp_Bishop;
            else if (this->WRook & from) piece = BoardPiece::bp_Rook;
            else if (this->WQueen & from) piece = BoardPiece::bp_Queen;
            else if (this->WKing & from) piece = BoardPiece::bp_King;
            else return false;
        }
        else {
            if (this->BPawn & from) piece = BoardPiece::bp_Pawn;
            else if (this->BKnight & from) piece = BoardPiece::bp_Knight;
            else if (this->BBishop & from) piece = BoardPiece::bp_Bishop;
            else if (this->BRook & from) piece = BoardPiece::bp_Rook;
            else if (this->BQueen & from) piece = BoardPiece::bp_Queen;
            else if (this->BKing & from) piece = BoardPiece::bp_King;
            else return false;
        }
        //get type
        unsigned int type = 0;
        if (promotion != BoardPiece::bp_Pawn) {
            type = 1; //0b0001 = promotion
        }
        else if (this->side) {
            //is castle right (short):
            if (lan == "e1g1" && piece == BoardPiece::bp_King && this->state.WCastleR) {
                type = 8; //0b1000
            }
            //is castle left (short):
            else if ((lan == "e1c1" && piece == BoardPiece::bp_King && this->state.WCastleL)) {
                type = 4; //0b0100
            }
            //is EP
            else if (this->state.hasEP) {
                if (this->EnPassantTarget << 8 == to && piece == BoardPiece::bp_Pawn) type = 2; //0b0010
            }
        }
        else {
            //is castle right (short):
            if (lan == "e8g8" && piece == BoardPiece::bp_King && this->state.BCastleR) {
                type = 8; //0b1000
            }
            //is castle left (short):
            else if ((lan == "e8c8" && piece == BoardPiece::bp_King && this->state.BCastleL)) {
                type = 4; //0b0100
            }
            //is EP
            else if (this->state.hasEP) {
                if (this->EnPassantTarget >> 8 == to && piece == BoardPiece::bp_Pawn) type = 2; //0b0010
            }
        }
        move.IsWhite = this->side;
        move.from = from;
        move.to = to;
        move.flags = encode_flags(type, piece, promotion);
        //return Move(this->side, from, to, encode_flags(type, piece, promotion));
        return true;
    }
    bool chess::ClassicBitBoard::Move2LAN(const Move& move, std::string& lan) {
        lan = Move::toLAN(move);
        return true;
    }
    bool chess::ClassicBitBoard::moveFromUCI(const std::string& lan, Move& move) {
        return LAN2Move(lan, move);
    }
    bool chess::ClassicBitBoard::SAN2Move(const std::string& san, Move& move) {
        std::vector<Move> moves;
        generate_moves(moves);
        //const std::regex re(R"(^([NBKRQ])?([a-h])?([1-8])?[\-x]?([a-h][1-8])(=?[nbrqkNBRQK])?(\+|#)?\Z)");

        //Note: "O-O+" is also a legal move! => solve for this
        if (san == "O-O" || san == "0-0"
            || san == "O-O+" || san == "0-0+"
            || san == "O-O#" || san == "0-0#") {
            //castle short == right
            for (auto m : moves) {
                if (getMoveType(m.flags) == 8) {
                    move = m;
                    return true;
                }
            }
            return false;
        }
        else if (san == "O-O-O" || san == "0-0-0"
            || san == "O-O-O+" || san == "0-0-0+"
            || san == "O-O-O#" || san == "0-0-0#") {
            //castle long == left
            for (auto m : moves) {
                if (getMoveType(m.flags) == 4) {
                    move = m;
                    return true;
                }
            }
            return false;
        }
        std::string match[7];
        const std::string pieces = "NBKRQ";
        const std::string files = "abcdefgh";
        const std::string ranks = "12345678";
        const std::string promotionTokens = "nbrqkNBRQK";
        const std::string endtokens = "+#";
        int i = (int)san.length() - 1;
        if (endtokens.find(san[i]) != std::string::npos) {
            match[6] = san[i];
            i--;

        }
        if (promotionTokens.find(san[i]) != std::string::npos) {
            if (san[i - 1] == '=') {
                match[5] = san[i];
                i -= 2;
            }
            else {//Normaly this should never be reached!
                match[5] = san[i];
                i--;
            }
        }
        if (ranks.find(san[i]) != std::string::npos) {
            if (files.find(san[i - 1]) != std::string::npos) {
                match[4] = san[i - 1];
                match[4] += san[i];
                i -= 2;
            }
        }
        if (i < 0) {
            return matchToMove(san, move, match, moves);
        }
        if (san[i] == 'x') {
            match[3] = "x";
            i--;
        }
        if (ranks.find(san[i]) != std::string::npos) {
            match[2] = san[i];
            i--;
        }
        if (files.find(san[i]) != std::string::npos) {
            match[1] = san[i];
            i--;
        }
        if (i < 0) {
            return matchToMove(san, move, match, moves);
        }
        if (pieces.find(san[i]) != std::string::npos) {
            match[0] = san[i];
            i--;
        }
        return matchToMove(san, move, match, moves);
    }
    bool chess::ClassicBitBoard::Move2SAN(const Move& move, std::string& san) {
        san = "";
        bool is_checkmate = false;
        bool is_check = false;
        //in_check after move?
        this->makeMove(move);
        is_check = this->isCheck();

        //in checkmate after move?
        if (is_check) { // only bother to calculate moves if check = true, otherwise it is never mate.
            std::vector<Move> moves;
            generate_moves(moves);
            if (moves.size() == 0) is_checkmate = true;
        }
        this->undoMove();

        //castling
        if (getMoveType(move.flags) == 8 || getMoveType(move.flags) == 4) {
            if (SquareOf(move.to) % 8 < SquareOf(move.from) % 8) {
                san = "O-O";
            }
            else {
                san = "O-O-O";
            }
            if (is_checkmate) {
                san += "#";
                return true;
            }
            else if (is_check) {
                san += "+";
                return true;
            }
            else {
                return true;
            }
        }
        map piece = getMoveOwnPiece(move.flags);
        bool is_capture = (((side) ? Enemy<true>() : Enemy<false>()) & move.to) ? true : false;
        if (piece == BoardPiece::bp_Pawn) {
            if (getMoveType(move.flags) == 2) { //EP capture!
                is_capture = true;
            }
        }
        else {
            san = "PNBRQK"[piece];
        }
        if (piece != BoardPiece::bp_Pawn) {
            uint64_t others = 0; //note: must be 64 bit for |= with m.from!
            std::vector<chess::Move> moves;
            generate_moves(moves);
            for (Move &m : moves) {
                if (m.to == move.to && m.from != move.from && getMoveOwnPiece(m.flags) == piece) { //other piece can move to the same position
                    others |= m.from;
                }
            }

            //Disambiguate
            if (others) {
                bool row = false, column = false;
                Bitloop(others) { 
                    // Multiple 'other' pieces with same target square (eg 3 rooks after promotion)
                    bit p = 1ull << SquareOf(others); // extract single piece from others;
                    if (getRank(p) == getRank(move.from)) {
                        column = true;
                    }
                    if (getFile(p) == getFile(move.from)) {
                        row = true;
                    }
                }
                if (!row && !column) {
                    column = true;
                }

                if (column)
                    san += algebraicFile(move.from);
                if (row)
                    san += algebraicRank(move.from);
            }
        }
        else if (is_capture) {
            san += algebraicFile(move.from);
        }

        //captures
        if (is_capture)
            san += "x";

        //desitnation
        san += algebraic(move.to);

        //promotion
        if (getMovePromote(move.flags)) {
            san += "=";
            san += "PNBRQK"[getMovePromote(move.flags)];
        }

        // is_checkmate:
        if (is_checkmate) {
            san += "#";
        }
        else if (is_check) {
            san += "+";
        }
        return true;
    }
    bool chess::ClassicBitBoard::Move2SAN(const Move& move, std::string& san, std::vector<Move>& generatedMoves) {
        san = "";
        bool is_checkmate = false;
        bool is_check = false;
        //in_check after move?
        this->makeMove(move);
        is_check = this->isCheck();

        //in checkmate after move?
        if (is_check) { // only bother to calculate moves if check = true, otherwise it is never mate.
            if (generatedMoves.size() == 0) is_checkmate = true;
        }
        this->undoMove();

        //castling
        if (getMoveType(move.flags) == 8 || getMoveType(move.flags) == 4) {
            if (SquareOf(move.to) % 8 < SquareOf(move.from) % 8) {
                san = "O-O";
            }
            else {
                san = "O-O-O";
            }
            if (is_checkmate) {
                san += "#";
                return true;
            }
            else if (is_check) {
                san += "+";
                return true;
            }
            else {
                return true;
            }
        }
        map piece = getMoveOwnPiece(move.flags);
        bool is_capture = (((side) ? Enemy<true>() : Enemy<false>()) & move.to) ? true : false;
        if (piece == BoardPiece::bp_Pawn) {
            if (getMoveType(move.flags) == 2) { //EP capture!
                is_capture = true;
            }
        }
        else {
            san = "PNBRQK"[piece];
        }
        if (piece != BoardPiece::bp_Pawn) {
            uint64_t others = 0; //note: must be 64 bit for |= with m.from!
            for (Move &m : generatedMoves) {
                if (m.to == move.to && m.from != move.from && getMoveOwnPiece(m.flags) == piece) { //other piece can move to the same position
                    others |= m.from;
                }
            }

            //Disambiguate
            if (others) {
                bool row = false, column = false;
                Bitloop(others) {
                    // Multiple 'ohter' pieces with same target square (eg 3 rooks after promotion)
                    bit p = 1ull << SquareOf(others); // extract single piece from others;
                    if (getRank(p) == getRank(move.from)) {
                        column = true;
                    }
                    if (getFile(p) == getFile(move.from)) {
                        row = true;
                    }
                }
                if (!row && !column) {
                    column = true;
                }

                if (column)
                    san += algebraicFile(move.from);
                if (row)
                    san += algebraicRank(move.from);
            }
        }
        else if (is_capture) {
            san += algebraicFile(move.from);
        }

        //captures
        if (is_capture)
            san += "x";

        //desitnation
        san += algebraic(move.to);

        //promotion
        if (getMovePromote(move.flags)) {
            san += "=";
            san += "PNBRQK"[getMovePromote(move.flags)];
        }

        // is_checkmate:
        if (is_checkmate) {
            san += "#";
        }
        else if (is_check) {
            san += "+";
        }
        return true;
    }
    
    //Depricated moves:
    Move chess::ClassicBitBoard::LAN2Move(std::string& LAN)
    {
        //split lan => always 4 characters present
        bit from = 1ull<<(('h' - LAN[0]) + (LAN[1] - '1') * 8);
        bit to = 1ull<<(('h' - LAN[2]) + (LAN[3] - '1') * 8);
        
        //get flags
        BoardPiece promotion = BoardPiece::bp_Pawn;
        if (LAN.size() == 5) {
            //promotion piece
            promotion = static_cast<BoardPiece>(std::string(" nbrq").find(LAN[4]));
        }
        //source piece:
        BoardPiece piece;
        if (this->side) {
            if (this->WPawn & from) piece = BoardPiece::bp_Pawn;
            else if (this->WKnight & from) piece = BoardPiece::bp_Knight;
            else if (this->WBishop & from) piece = BoardPiece::bp_Bishop;
            else if (this->WRook & from) piece = BoardPiece::bp_Rook;
            else if (this->WQueen & from) piece = BoardPiece::bp_Queen;
            else if (this->WKing & from) piece = BoardPiece::bp_King;
            else throw new std::invalid_argument(LAN);
        }
        else {
            if (this->BPawn & from) piece = BoardPiece::bp_Pawn;
            else if (this->BKnight & from) piece = BoardPiece::bp_Knight;
            else if (this->BBishop & from) piece = BoardPiece::bp_Bishop;
            else if (this->BRook & from) piece = BoardPiece::bp_Rook;
            else if (this->BQueen & from) piece = BoardPiece::bp_Queen;
            else if (this->BKing & from) piece = BoardPiece::bp_King;
            else throw new std::invalid_argument(LAN);
        }
        //get type
        unsigned int type = 0;
        if (promotion != BoardPiece::bp_Pawn) {
            type = 1; //0b0001 = promotion
        }
        else if (this->side) {
            //is castle right (short):
            if (LAN == "e1g1" && piece == BoardPiece::bp_King && this->state.WCastleR) {
                type = 8; //0b1000
            }
            //is castle left (short):
            else if ((LAN == "e1c1" && piece == BoardPiece::bp_King && this->state.WCastleL)) {
                type = 4; //0b0100
            }
            //is EP
            else if (this->state.hasEP) {
                if (this->EnPassantTarget << 8 == to && piece == BoardPiece::bp_Pawn) type = 2; //0b0010
            }
        }
        else {
            //is castle right (short):
            if (LAN == "e8g8" && piece == BoardPiece::bp_King && this->state.BCastleR) {
                type = 8; //0b1000
            }
            //is castle left (short):
            else if ((LAN == "e8c8" && piece == BoardPiece::bp_King && this->state.BCastleL)) {
                type = 4; //0b0100
            }
            //is EP
            else if (this->state.hasEP) {
                if (this->EnPassantTarget >> 8 == to && piece == BoardPiece::bp_Pawn) type = 2; //0b0010
            }
        }
        return Move(this->side, from, to,encode_flags(type,piece,promotion));
    }
    std::string chess::ClassicBitBoard::Move2LAN(Move& move)
    {
        return move.toLAN();
    }
    Move chess::ClassicBitBoard::moveFromUCI(std::string& lan)
    {
        return LAN2Move(lan);
    }
    Move chess::ClassicBitBoard::SAN2Move(std::string& SAN) {
        std::vector<Move> moves;
        generate_moves(moves);
        /*std::vector<std::string> sanmoves;
        sanmoves.reserve(moves.size());
        for (auto m : moves) {
            sanmoves.push_back(MovetoSAN(m, moves));
            if (SAN == sanmoves.back()) {
                return m;
            }
        }*/
        //If no common move was found => maybe some special symbols are different:
        //std::regex re("^([NBKRQ])?([a-h])?([1-8])?[\\-x]?([a-h][1-8])(=?[nbrqkNBRQK])?(\\+|#)?\\Z");
        //const std::regex re(R"(^([NBKRQ])?([a-h])?([1-8])?[\-x]?([a-h][1-8])(=?[nbrqkNBRQK])?(\+|#)?\Z)");
        //std::smatch match;
        //bool test = std::regex_search(SAN, match, re);

        //Note: "O-O+" is also a legal move! => solve for this
        if (SAN == "O-O" || SAN == "0-0"
            || SAN == "O-O+" || SAN == "0-0+"
            || SAN == "O-O#" || SAN == "0-0#") {
            //castle short == right
            for (auto move : moves) {
                if (getMoveType(move.flags) == 8) {
                    return move;
                }
            }
            throw new std::invalid_argument(SAN);
        }
        else if (SAN == "O-O-O" || SAN== "0-0-0" 
            || SAN == "O-O-O+" || SAN == "0-0-0+"
            || SAN == "O-O-O#" || SAN == "0-0-0#") {
            //castle long == left
            for (auto move : moves) {
                if (getMoveType(move.flags) == 4) {
                    return move;
                }
            }
            throw new std::invalid_argument(SAN);
        }
        std::string match[7];
        const std::string pieces = "NBKRQ";
        const std::string files = "abcdefgh";
        const std::string ranks = "12345678";
        const std::string promotionTokens = "nbrqkNBRQK";
        const std::string endtokens = "+#";
        int i = (int)SAN.length()-1;
        if (endtokens.find(SAN[i]) != std::string::npos) {
            match[6] = SAN[i];
            i--;
            
        }
        if (promotionTokens.find(SAN[i]) != std::string::npos) {
            if (SAN[i - 1] == '=') {
                match[5] = SAN[i];
                i -= 2;
            }
            else {//Normaly this should never be reached!
                match[5] = SAN[i];
                i--;
            }
        }
        if (ranks.find(SAN[i]) != std::string::npos) {
            if (files.find(SAN[i - 1]) != std::string::npos) {
                match[4] = SAN[i - 1];
                match[4]+= SAN[i];
                i -= 2;
            }
        }
        if (i < 0) {
            return matchToMove(SAN,match, moves);
        }
        if (SAN[i] == 'x') {
            match[3] = "x";
            i--;
        }
        if (ranks.find(SAN[i]) != std::string::npos) {
            match[2] = SAN[i];
            i--;
        }
        if (files.find(SAN[i]) != std::string::npos) {
            match[1] = SAN[i];
            i--;
        }
        if (i < 0) {
            return matchToMove(SAN,match, moves);
        }
        if (pieces.find(SAN[i]) != std::string::npos) {
            match[0] = SAN[i];
            i--;
        }
        return matchToMove(SAN,match, moves);
    }
    std::string chess::ClassicBitBoard::Move2SAN(Move& move)
    {
        std::string san;
        bool is_checkmate = false;
        bool is_check = false;
        //in_check after move?
        this->makeMove(move);
        is_check = this->isCheck();
        
        //in checkmate after move?
        if (is_check) { // only bother to calculate moves if check = true, otherwise it is never mate.
            std::vector<Move> moves;
            generate_moves(moves);
            if (moves.size() == 0) is_checkmate = true;
        }
        this->undoMove();

        //castling
        if (getMoveType(move.flags) == 8 || getMoveType(move.flags) == 4) {
            if (SquareOf(move.to) % 8 < SquareOf(move.from) % 8) {
                san = "O-O";
            }
            else {
                san = "O-O-O";
            }
            if (is_checkmate) {
                return san + "#";
            }
            else if (is_check){
                return san + "+";
            }
            else {
                return san;
            }
        }
        map piece = getMoveOwnPiece(move.flags);
        bool is_capture = (((side) ? Enemy<true>() : Enemy<false>()) & move.to) ? true : false;          
        if (piece == BoardPiece::bp_Pawn) {
            san = "";
            if (getMoveType(move.flags) == 2) { //EP capture!
                is_capture = true;
            }
        }
        else {
            san = "PNBRQK"[piece];        
        }
        if (piece != BoardPiece::bp_Pawn){
            uint64_t others = 0; //note: must be 64 bit for |= with m.from!
            std::vector<chess::Move> moves;
            generate_moves(moves);
            for (Move &m : moves) {
                if (m.to == move.to && m.from != move.from && getMoveOwnPiece(m.flags) == piece) { //other piece can move to the same position
                    others |= m.from;
                }
            }

            //Disambiguate
            if (others) {
                bool row = false, column = false;
                /*old code: works if only 1 other piece is pressent, below also takes rare state with >2 other pieces from the same type and the same target*/
                /*if (getRank(p) == getRank(move.from)) {
                    column = true;
                }
                else if (getFile(p) == getFile(move.from)) {
                    row = true;
                }
                else {
                    column = true;
                }*/
                Bitloop(others) { //Multiple 'ohter' pieces with same target square (eg 3 rooks after promotion)
                    bit p = 1ull << SquareOf(others); //extract single piece from others;
                    if (getRank(p) == getRank(move.from)) {
                        column = true;
                    }
                    if (getFile(p) == getFile(move.from)) {
                        row = true;
                    }
                    //if (column || row) {
                    //    // only brake after both conditions checked => multiple other pieces where one is placed on the same file and one 
                    //    // on the same rank must be able to set both flags.
                    //    break;
                    //}   
                }
                if (!row && !column) {
                    column = true;
                }
                
                if (column)
                    san += algebraicFile(move.from);
                if (row)
                    san += algebraicRank(move.from);

            }
        }
        else if (is_capture) {
            san += algebraicFile(move.from);
        }
        
        //captures
        if (is_capture)
            san += "x";
        
        //desitnation
        san += algebraic(move.to);

        //promotion
        if (getMovePromote(move.flags)) {
            san += "=";
            san += "PNBRQK"[getMovePromote(move.flags)];
        }

        // is_checkmate:
        if (is_checkmate) {
            san += "#";
        }
        else if (is_check) {
            san += "+";
        }        
        return san; 
    }
    std::string chess::ClassicBitBoard::Move2SAN(Move& move, std::vector<Move>& generatedMoves) {
        std::string san;
        bool is_checkmate = false;
        bool is_check = false;
        //in_check after move?
        this->makeMove(move);
        is_check = this->isCheck();

        //in checkmate after move?
        if (is_check) { // only bother to calculate moves if check = true, otherwise it is never mate.
            if (generatedMoves.size() == 0) is_checkmate = true;
        }
        this->undoMove();

        //castling
        if (getMoveType(move.flags) == 8 || getMoveType(move.flags) == 4) {
            if (SquareOf(move.to) % 8 < SquareOf(move.from) % 8) {
                san = "O-O";
            }
            else {
                san = "O-O-O";
            }
            if (is_checkmate) {
                return san + "#";
            }
            else if (is_check) {
                return san + "+";
            }
            else {
                return san;
            }
        }
        map piece = getMoveOwnPiece(move.flags);
        bool is_capture = (((side) ? Enemy<true>() : Enemy<false>()) & move.to) ? true : false;
        if (piece == BoardPiece::bp_Pawn) {
            san = "";
            if (getMoveType(move.flags) == 2) { //EP capture!
                is_capture = true;
            }
        }
        else {
            san = "PNBRQK"[piece];
        }
        if (piece != BoardPiece::bp_Pawn) {
            uint64_t others = 0; //note: must be 64 bit for |= with m.from!
            for (Move const &m : generatedMoves) {
                if (m.to == move.to && m.from != move.from && getMoveOwnPiece(m.flags) == piece) { //other piece can move to the same position
                    others |= m.from;
                }
            }

            //Disambiguate
            if (others) {
                bool row = false, column = false;
                Bitloop(others) { //Multiple 'ohter' pieces with same target square (eg 3 rooks after promotion)
                    bit p = 1ull << SquareOf(others); //extract single piece from others;
                    if (getRank(p) == getRank(move.from)) {
                        column = true;
                    }
                    if (getFile(p) == getFile(move.from)) {
                        row = true;
                    }
                }
                if (!row && !column) {
                    column = true;
                }

                if (column)
                    san += algebraicFile(move.from);
                if (row)
                    san += algebraicRank(move.from);
            }
        }
        else if (is_capture) {
            san += algebraicFile(move.from);
        }

        //captures
        if (is_capture)
            san += "x";

        //desitnation
        san += algebraic(move.to);

        //promotion
        if (getMovePromote(move.flags)) {
            san += "=";
            san += "PNBRQK"[getMovePromote(move.flags)];
        }

        // is_checkmate:
        if (is_checkmate) {
            san += "#";
        }
        else if (is_check) {
            san += "+";
        }
        return san;
    }
    // end Depricated methods
    
    uint64_t ClassicBitBoard::hash() {
        return 0;
    }

    void ClassicBitBoard::reset() {
        parse_fen(chess::ClassicBitBoard::startpos);
        White = WPawn | WBishop | WKnight | WRook | WKing | WQueen;
        Black = BPawn | BBishop | BKnight | BRook | BKing | BQueen;
        Occ = White | Black;
        this->boardHistory.clear();
    }
    void chess::ClassicBitBoard::set(std::string fen)
    {
        parse_fen(fen);
        White = WPawn | WBishop | WKnight | WRook | WKing | WQueen;
        Black = BPawn | BBishop | BKnight | BRook | BKing | BQueen;
        Occ = White | Black;
        this->boardHistory.clear();
    }
    bool chess::ClassicBitBoard::pgnParser(std::string pgn, std::vector<Move>& moves)
    {
        //pgn must be stripped, only 1 line containing all moves!
        std::string pgnMove;
        std::vector<std::string> SAN;
        Move temp;
        SAN.reserve(250); //OTB-HQ => 1001 games with 125+ moves in 64% of the file.
        std::stringstream ss;
        ss << pgn;

        while (std::getline(ss, pgnMove, ' ')) {
            size_t index = pgnMove.find('.');
            // if '.' found => move number sitll present  => remove move nmuber
            pgnMove = pgnMove.substr(index+1);
            if (pgnMove != "") SAN.push_back(pgnMove); //remove blankspaces
        }
        ClassicBitBoard board = ClassicBitBoard();
       
        //SAN contains all SAN Moves
        moves.clear();
        moves.reserve(SAN.size());
        for (auto &m : SAN) {
            if (board.SAN2Move(m, temp)) {
                moves.push_back(temp);
                board.makeMove(moves.back());
            }
            else {
                //parse errror => return
                return false;
            }            
        }
        return true;
    }
   
    Move ClassicBitBoard::matchToMove(const std::string &SAN, std::string match[], std::vector<Move>& moves) {
        square to_square = ((uint64_t)'h' - match[4][0]) + (match[4][1] - (uint64_t)'1') * 8;
        bit to_mask = 1ull << to_square;
        map from_mask;
        const std::string pieces = " NBRQK"; // same order as BoardPieces (index = enum) 
        int p = (int)pieces.find(std::toupper(match[5][0]));
        
        BoardPiece type = BoardPiece::bp_Pawn;
        //filter by piece type:
        if (match[0] != "") {
            int index = (int)pieces.find(std::toupper(match[0][0]));
            switch (index) {
            case 1:
                type = BoardPiece::bp_Knight;
                from_mask = side ? Knights<true>() : Knights<false>();
                break;
            case 2:
                type = BoardPiece::bp_Bishop;
                from_mask = side ? Bishops<true>() : Bishops<false>();
                break;
            case 3:
                type = BoardPiece::bp_Rook;
                from_mask = side ? Rooks<true>() : Rooks<false>();
                break;
            case 4:
                type = BoardPiece::bp_Queen;
                from_mask = side ? Queens<true>() : Queens<false>();
                break;
            case 5:
                type = BoardPiece::bp_King;
                from_mask = side ? King<true>() : King<false>();
                break;
            }
        }
        else {
            type = BoardPiece::bp_Pawn;
            from_mask = side ? Pawns<true>() : Pawns<false>();
        }
        
        if (match[1] != "") {
            int fileIndex = 'h' - match[1][0];
            from_mask &= File8<<fileIndex;
        }
        if (match[2] != "") {
            int rankIndex = match[2][0] - '1';
            from_mask &= Rank1 << rankIndex*8;
        }

        int matched_move = -1;
        for (int i = 0; i < moves.size();i++) {
            if (getMovePromote(moves[i].flags) != p && p!=-1) {
                continue;
            }
            auto test1 = getMoveOwnPiece(moves[i].flags);
            auto test2 = moves[i].to;
            auto test3 = moves[i].from & from_mask;
            if (moves[i].to == to_mask && getMoveOwnPiece(moves[i].flags) == type) {
                if (moves[i].from & from_mask) { //if zero => not the same
                    matched_move = i;
                }
            }
        }
        if (matched_move == -1) throw new std::invalid_argument(SAN);
        return moves[matched_move];
    } 
    bool ClassicBitBoard::matchToMove(const std::string &SAN, Move& move, std::string match[], std::vector<Move>& moves) {
        square to_square = ((uint64_t)'h' - match[4][0]) + (match[4][1] - (uint64_t)'1') * 8;
        bit to_mask = 1ull << to_square;
        map from_mask;
        const std::string pieces = " NBRQK"; // same order as BoardPieces (index = enum) 
        int p = (int)pieces.find(std::toupper(match[5][0]));
        
        BoardPiece type = BoardPiece::bp_Pawn;
        //filter by piece type:
        if (match[0] != "") {
            int index = (int)pieces.find(std::toupper(match[0][0]));
            switch (index) {
            case 1:
                type = BoardPiece::bp_Knight;
                from_mask = side ? Knights<true>() : Knights<false>();
                break;
            case 2:
                type = BoardPiece::bp_Bishop;
                from_mask = side ? Bishops<true>() : Bishops<false>();
                break;
            case 3:
                type = BoardPiece::bp_Rook;
                from_mask = side ? Rooks<true>() : Rooks<false>();
                break;
            case 4:
                type = BoardPiece::bp_Queen;
                from_mask = side ? Queens<true>() : Queens<false>();
                break;
            case 5:
                type = BoardPiece::bp_King;
                from_mask = side ? King<true>() : King<false>();
                break;
            }
        }
        else {
            type = BoardPiece::bp_Pawn;
            from_mask = side ? Pawns<true>() : Pawns<false>();
        }
        
        if (match[1] != "") {
            int fileIndex = 'h' - match[1][0];
            from_mask &= File8<<fileIndex;
        }
        if (match[2] != "") {
            int rankIndex = match[2][0] - '1';
            from_mask &= Rank1 << rankIndex*8;
        }

        int matched_move = -1;
        for (int i = 0; i < moves.size();i++) {
            if (getMovePromote(moves[i].flags) != p && p!=-1) {
                continue;
            }
            auto test1 = getMoveOwnPiece(moves[i].flags);
            auto test2 = moves[i].to;
            auto test3 = moves[i].from & from_mask;
            if (moves[i].to == to_mask && getMoveOwnPiece(moves[i].flags) == type) {
                if (moves[i].from & from_mask) { //if zero => not the same
                    matched_move = i;
                }
            }
        }
        if (matched_move == -1) return false;
        move = moves[matched_move];
        return true;
    }

    template<bool IsWhite>
    constexpr uint64_t ClassicBitBoard::Castle_RookswitchR() {
        if (IsWhite) return 0b00000101ull;
        else return 0b00000101ull << 56;
    }
    template<bool IsWhite>
    constexpr uint64_t ClassicBitBoard::Castle_RookswitchL() {
        if (IsWhite) return 0b10010000ull;
        else return 0b10010000ull << 56;
    }
    template<bool IsWhite, bool newEP>
    _InlineConstExpr void ClassicBitBoard::updateState(const Move& move)
    {
        if constexpr (newEP) {
            state.hasEP = true;
            IsWhite ? (this->EnPassantTarget = move.to) : (this->EnPassantTarget = move.to);
        }
        else {
            state.hasEP = false;
            EnPassantTarget = 0;
        }
        if constexpr (IsWhite) {
            if (move.from == state.WRookL || move.from == state.WKing) state.WCastleL = false; //if rookL moves at least once => clear flag
            if (move.from == state.WRookR || move.from == state.WKing) state.WCastleR = false; //if rookR moves at least once => clear flag
            if (move.to == state.BRookL) state.BCastleL = false; //if opponent rookL is caputerd => clear flag (note: if no rook is present => flag already cleared
            if (move.to == state.BRookR) state.BCastleR = false; //if opponent rookR is caputerd => clear flag (note: if no rook is present => flag already cleared
        }
        else {
            if (move.from == state.BRookL || move.from == state.BKing) state.BCastleL = false; //if rookL moves at least once => clear flag
            if (move.from == state.BRookR || move.from == state.BKing) state.BCastleR = false; //if rookR moves at least once => clear flag
            if (move.to == state.WRookL) state.WCastleL = false; //if opponent rookL is caputerd => clear flag (note: if no rook is present => flag already cleared
            if (move.to == state.WRookR) state.WCastleR = false; //if opponent rookR is caputerd => clear flag (note: if no rook is present => flag already cleared
        }
    }

    template<bool IsWhite, bool inCheck>
    __forceinline void chess::ClassicBitBoard::__generate_capture_moves(map kingatk, const map kingban, const map Checkmask, std::vector<Move>& moves)
    {
        constexpr bool white = IsWhite;
        uint64_t movecnt = 0;

        const map pinHV = rookPin;
        const map pinD12 = bishopPin;
        const map checkmask = ConstCheckmask<inCheck>(Checkmask);
        const map movableSquare = MoveableSquares<white, inCheck>(checkmask);
        const map captureSquare = movableSquare & Enemy<IsWhite>();
        const map epTarget = EnPassantTarget;


        //Pawns
        {
            const uint64_t pawnsLR = Pawns<white>() & ~pinHV;
            const uint64_t pawnsHV = Pawns<white>() & ~pinD12;

            //4 basic pawn moves => only 2 remain (Left and Right) for capture moves
            uint64_t Lpawns = pawnsLR & Pawn_InvertLeft<white>(Enemy<white>() & Pawns_NotRight() & checkmask);
            uint64_t Rpawns = pawnsLR & Pawn_InvertRight<white>(Enemy<white>() & Pawns_NotLeft() & checkmask);


            Pawn_PruneLeft<white>(Lpawns, pinD12);
            Pawn_PruneRight<white>(Rpawns, pinD12);

            //Enpassant
            if (state.hasEP) { //EP = always capture!
                bit EPLpawn = pawnsLR & Pawns_NotLeft() & ((epTarget & checkmask) >> 1);
                bit EPRpawn = pawnsLR & Pawns_NotRight() & ((epTarget & checkmask) << 1);

                if (EPLpawn | EPRpawn) {
                    Pawn_PruneLeftEP<white>(EPLpawn, pinD12);
                    Pawn_PruneRightEP<white>(EPRpawn, pinD12);
                    if (EPLpawn) {
                        //Callback_Move::template PawnEnpassantTake<status, depth>(brd, EPLpawn, EPLpawn << 1, Pawn_AttackLeft<white>(EPLpawn));
                        moves.emplace_back(IsWhite, EPLpawn, Pawn_AttackLeft<white>(EPLpawn),
                                           encode_flags(0b0010, BoardPiece::bp_Pawn, 0));
                    }
                    if (EPRpawn) {
                        //Callback_Move::template PawnEnpassantTake<status, depth>(brd, EPRpawn, EPRpawn >> 1, Pawn_AttackRight<white>(EPRpawn));
                        moves.emplace_back(IsWhite, EPRpawn, Pawn_AttackRight<white>(EPRpawn),
                                           encode_flags(0b0010, BoardPiece::bp_Pawn, 0));
                    }
                }
            }

            //Pawns that can move on te last rank:
            if ((Lpawns | Rpawns) & Pawns_LastRank<white>()) {
                uint64_t Promote_Left = Lpawns & Pawns_LastRank<white>();
                uint64_t Promote_Right = Rpawns & Pawns_LastRank<white>();

                uint64_t NoPromote_Left = Lpawns & ~Pawns_LastRank<white>();
                uint64_t NoPromote_Right = Rpawns & ~Pawns_LastRank<white>();

                while (Promote_Left) {
                    const bit pos = PopBit(Promote_Left);
                    //Callback_Move::template Pawnpromote<status, depth>(brd, pos, Pawn_AttackLeft<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Queen));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Bishop));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Rook));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Knight));
                }
                while (Promote_Right) {
                    const bit pos = PopBit(Promote_Right);
                    //Callback_Move::template Pawnpromote<status, depth>(brd, pos, Pawn_AttackRight<white>(pos)); 
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Queen));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Bishop));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Rook));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Knight));
                }
                while (NoPromote_Left) {
                    const bit pos = PopBit(NoPromote_Left);
                    //Callback_Move::template Pawnatk<status, depth>(brd, pos, Pawn_AttackLeft<white>(pos)); 
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
                while (NoPromote_Right) {
                    const bit pos = PopBit(NoPromote_Right);
                    //Callback_Move::template Pawnatk<status, depth>(brd, pos, Pawn_AttackRight<white>(pos)); 
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
            } else {
                while (Lpawns) {
                    const bit pos = PopBit(Lpawns);
                    //Callback_Move::template Pawnatk<status, depth>(brd, pos, Pawn_AttackLeft<white>(pos)); 
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
                while (Rpawns) {
                    const bit pos = PopBit(Rpawns);
                    //Callback_Move::template Pawnatk<status, depth>(brd, pos, Pawn_AttackRight<white>(pos)); 
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
            }
        }
        //Knightmoves
        {
            map knights = Knights<white>() & ~(pinHV | pinD12);
            Bitloop(knights) {
                const square sq = SquareOf(knights);
                map move = lookup::Knight(sq) & captureSquare;
                while (move) {
                    const bit to = PopBit(move);
                    //Callback_Move::template Knightmove<status, depth>(brd, 1ull << sq, to);
                    moves.emplace_back(IsWhite, 1ull << sq, to, encode_flags(0b0000, BoardPiece::bp_Knight, 0));

                }
            }
        }

        //bishops (and diag pinned queens)
        const map queens = Queens<white>();
        {
            map bishops = Bishops<white>() & ~pinHV;

            map bish_pinned = (bishops | queens) & pinD12;
            map bish_nopin = bishops & ~pinD12;

            Bitloop(bish_pinned) {
                const square sq = SquareOf(bish_pinned);
                map move = lookup::Bishop(sq, Occ) & captureSquare & pinD12;

                map pos = 1ull << sq;
                if (pos & queens) {
                    while (move) {
                        const bit to = PopBit(move);
                        //Callback_Move::template Queenmove<status, depth>(brd, pos, to);
                        moves.emplace_back(IsWhite, pos, to, encode_flags(0b0000, BoardPiece::bp_Queen, 0));
                    }
                }
                else {
                    while (move) {
                        const bit to = PopBit(move);
                        //Callback_Move::template Bishopmove<status, depth>(brd, pos, to);
                        moves.emplace_back(IsWhite, pos, to, encode_flags(0b0000, BoardPiece::bp_Bishop, 0));
                    }
                }
            }
            Bitloop(bish_nopin) {
                const square sq = SquareOf(bish_nopin);
                map move = lookup::Bishop(sq, Occ) & captureSquare;
                while (move) {
                    const bit to = PopBit(move);
                    //Callback_Move::template Bishopmove<status, depth>(brd, 1ull << sq, to);
                    moves.emplace_back(IsWhite, 1ull << sq, to, encode_flags(0b0000, BoardPiece::bp_Bishop, 0));
                }
            }
        }

        //Rookmoves (and horiz pinned queens)
        {
            map rooks = Rooks<white>() & ~pinD12;

            map rook_pinned = (rooks | queens) & pinHV;
            map rook_nopin = rooks & ~pinHV;
            Bitloop(rook_pinned) {
                const square sq = SquareOf(rook_pinned);
                map move = lookup::Rook(sq, Occ) & captureSquare & pinHV;
                map pos = 1ull << sq;
                if (pos & queens) {
                    while (move) {
                        const bit to = PopBit(move);
                        //Callback_Move::template Queenmove<status, depth>(brd, pos, to);
                        moves.emplace_back(IsWhite, pos, to, encode_flags(0b0000, BoardPiece::bp_Queen, 0));
                    }
                }
                else {
                    while (move) {
                        const bit to = PopBit(move);
                        //Callback_Move::template Rookmove<status, depth>(brd, pos, to);
                        moves.emplace_back(IsWhite, pos, to, encode_flags(0b0000, BoardPiece::bp_Rook, 0));
                    }
                }
            }
            Bitloop(rook_nopin) {
                    const square sq = SquareOf(rook_nopin);
                    map move = lookup::Rook(sq, Occ) & captureSquare;
                while (move) {
                    const bit to = PopBit(move);
                    //Callback_Move::template Rookmove<status, depth>(brd, 1ull << sq, to);
                    moves.emplace_back(IsWhite, 1ull << sq, to, encode_flags(0b0000, BoardPiece::bp_Rook, 0));
                }
            }
        }

        //Queen moves
        {
            map queens = Queens<white>() & ~(pinHV | pinD12);
            Bitloop(queens) {
                const square sq = SquareOf(queens);
                map atk = lookup::Queen(sq, Occ);
                map move = atk & captureSquare;
                while (move) {
                    const bit to = PopBit(move);
                    //Callback_Move::template Queenmove<status, depth>(brd, 1ull << sq, to);
                    moves.emplace_back(IsWhite, 1ull << sq, to, encode_flags(0b0000, BoardPiece::bp_Queen, 0));
                }
            }
        }
        //KingMoves (put last => lower priority when going over all the moves, other moves are probably better and can cause an earlier cut off
        {
            Bitloop(kingatk) {
                const square sq = SquareOf(kingatk);
                if (1ull << sq & Enemy<IsWhite>()) {
                    //only capture moves => enemy must be present at target location
                    moves.emplace_back(IsWhite, King<IsWhite>(), 1ull << sq,
                                       encode_flags(0b0000, BoardPiece::bp_King, 0));
                }
            }
            //Castling => never capture

        }
        return;
    }

    template<bool IsWhite, bool inCheck>
    __forceinline void chess::ClassicBitBoard::__generate_moves(map kingatk, const map kingban, const map Checkmask, std::vector<Move>& moves) {
        constexpr bool white = IsWhite;
        uint64_t movecnt = 0;

        const map pinHV = rookPin;
        const map pinD12 = bishopPin;
        const map checkmask = ConstCheckmask<inCheck>(Checkmask);
        const map movableSquare = MoveableSquares<white, inCheck>(checkmask);
        const map epTarget = EnPassantTarget;


        //Pawns
        {
            const uint64_t pawnsLR = Pawns<white>() & ~pinHV;
            const uint64_t pawnsHV = Pawns<white>() & ~pinD12;

            //4 basic pawn moves
            uint64_t Lpawns = pawnsLR & Pawn_InvertLeft<white>(Enemy<white>() & Pawns_NotRight() & checkmask);
            uint64_t Rpawns = pawnsLR & Pawn_InvertRight<white>(Enemy<white>() & Pawns_NotLeft() & checkmask);
            uint64_t Fpawns = pawnsHV & Pawn_Backward<white>(Empty());
            uint64_t Ppawns = Fpawns & Pawns_FirstRank<white>() & Pawn_Backward2<white>(Empty() & checkmask);

            Fpawns &= Pawn_Backward<white>(checkmask);

            Pawn_PruneLeft<white>(Lpawns, pinD12);
            Pawn_PruneRight<white>(Rpawns, pinD12);
            Pawn_PruneMove<white>(Fpawns, pinHV);
            Pawn_PruneMove2<white>(Ppawns, pinHV);

            //Enpassant
            if (state.hasEP) {
                bit EPLpawn = pawnsLR & Pawns_NotLeft() & ((epTarget & checkmask) >> 1);
                bit EPRpawn = pawnsLR & Pawns_NotRight() & ((epTarget & checkmask) << 1);

                if (EPLpawn | EPRpawn) {
                    Pawn_PruneLeftEP<white>(EPLpawn, pinD12);
                    Pawn_PruneRightEP<white>(EPRpawn, pinD12);
                    if (EPLpawn) {
                        //Callback_Move::template PawnEnpassantTake<status, depth>(brd, EPLpawn, EPLpawn << 1, Pawn_AttackLeft<white>(EPLpawn));
                        moves.emplace_back(IsWhite, EPLpawn, Pawn_AttackLeft<white>(EPLpawn),
                                           encode_flags(0b0010, BoardPiece::bp_Pawn, 0));
                    }
                    if (EPRpawn) {
                        //Callback_Move::template PawnEnpassantTake<status, depth>(brd, EPRpawn, EPRpawn >> 1, Pawn_AttackRight<white>(EPRpawn));
                        moves.emplace_back(IsWhite, EPRpawn, Pawn_AttackRight<white>(EPRpawn),
                                           encode_flags(0b0010, BoardPiece::bp_Pawn, 0));
                    }
                }
            }

            //Pawns that can move on te last rank:
            if ((Lpawns | Rpawns | Fpawns) & Pawns_LastRank<white>()) {
                uint64_t Promote_Left = Lpawns & Pawns_LastRank<white>();
                uint64_t Promote_Right = Rpawns & Pawns_LastRank<white>();
                uint64_t Promote_Move = Fpawns & Pawns_LastRank<white>();

                uint64_t NoPromote_Left = Lpawns & ~Pawns_LastRank<white>();
                uint64_t NoPromote_Right = Rpawns & ~Pawns_LastRank<white>();
                uint64_t NoPromote_Move = Fpawns & ~Pawns_LastRank<white>();

                while (Promote_Left) {
                    const bit pos = PopBit(Promote_Left);
                    //Callback_Move::template Pawnpromote<status, depth>(brd, pos, Pawn_AttackLeft<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Queen));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Bishop));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Rook));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Knight));
                }
                while (Promote_Right) {
                    const bit pos = PopBit(Promote_Right);
                    //Callback_Move::template Pawnpromote<status, depth>(brd, pos, Pawn_AttackRight<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Queen));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Bishop));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Rook));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Knight));
                }
                while (Promote_Move) {
                    const bit pos = PopBit(Promote_Move);
                    //Callback_Move::template Pawnpromote<status, depth>(brd, pos, Pawn_Forward<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_Forward<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Queen));
                    moves.emplace_back(IsWhite, pos, Pawn_Forward<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Bishop));
                    moves.emplace_back(IsWhite, pos, Pawn_Forward<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Rook));
                    moves.emplace_back(IsWhite, pos, Pawn_Forward<white>(pos),
                                       encode_flags(0b0001, BoardPiece::bp_Pawn, BoardPiece::bp_Knight));
                }
                while (NoPromote_Left) {
                    const bit pos = PopBit(NoPromote_Left);
                    //Callback_Move::template Pawnatk<status, depth>(brd, pos, Pawn_AttackLeft<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
                while (NoPromote_Right) {
                    const bit pos = PopBit(NoPromote_Right);
                    //Callback_Move::template Pawnatk<status, depth>(brd, pos, Pawn_AttackRight<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
                while (NoPromote_Move) {
                    const bit pos = PopBit(NoPromote_Move);
                    //Callback_Move::template Pawnmove<status, depth>(brd, pos, Pawn_Forward<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_Forward<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
                while (Ppawns) {
                    const bit pos = PopBit(Ppawns);
                    //Callback_Move::template Pawnpush<status, depth>(brd, pos, Pawn_Forward2<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_Forward2<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
            } else {
                while (Lpawns) {
                    const bit pos = PopBit(Lpawns);
                    //Callback_Move::template Pawnatk<status, depth>(brd, pos, Pawn_AttackLeft<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackLeft<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
                while (Rpawns) {
                    const bit pos = PopBit(Rpawns);
                    //Callback_Move::template Pawnatk<status, depth>(brd, pos, Pawn_AttackRight<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_AttackRight<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
                while (Fpawns) {
                    const bit pos = PopBit(Fpawns);
                    //Callback_Move::template Pawnmove<status, depth>(brd, pos, Pawn_Forward<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_Forward<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
                while (Ppawns) {
                    const bit pos = PopBit(Ppawns);
                    //Callback_Move::template Pawnpush<status, depth>(brd, pos, Pawn_Forward2<white>(pos));
                    moves.emplace_back(IsWhite, pos, Pawn_Forward2<white>(pos),
                                       encode_flags(0b0000, BoardPiece::bp_Pawn, 0));
                }
            }
        }

        //Castling
        {
            if constexpr (!inCheck) {
                if (state.canCastleLeft<white>(kingban, Occ, Rooks<white>())) {
                    //Callback_Move::template KingCastle<status, depth>(brd, (King<white>(brd) | King<white>(brd) << 2), status.Castle_RookswitchL());
                    moves.emplace_back(IsWhite, King<IsWhite>(), King<IsWhite>() << 2,
                                       encode_flags(0b0100, BoardPiece::bp_King, 0));
                }
                if (state.canCastleRight<white>(kingban, Occ, Rooks<white>())) {
                    //Callback_Move::template KingCastle<status, depth>(brd, (King<white>(brd) | King<white>(brd) >> 2), status.Castle_RookswitchR());
                    moves.emplace_back(IsWhite, King<IsWhite>(), King<IsWhite>() >> 2,
                                       encode_flags(0b1000, BoardPiece::bp_King, 0));
                }
            }
        }
        //Knightmoves
        {
            map knights = Knights<white>() & ~(pinHV | pinD12);
            Bitloop(knights) {
                const square sq = SquareOf(knights);
                map move = lookup::Knight(sq) & movableSquare;
                while (move) {
                    const bit to = PopBit(move);
                    //Callback_Move::template Knightmove<status, depth>(brd, 1ull << sq, to);
                    moves.emplace_back(IsWhite, 1ull << sq, to, encode_flags(0b0000, BoardPiece::bp_Knight, 0));

                }
            }
        }

        //bishops (and diag pinned queens)
        const map queens = Queens<white>();
        {
            map bishops = Bishops<white>() & ~pinHV;

            map bish_pinned = (bishops | queens) & pinD12;
            map bish_nopin = bishops & ~pinD12;

            Bitloop(bish_pinned) {
                const square sq = SquareOf(bish_pinned);
                map move = lookup::Bishop(sq, Occ) & movableSquare & pinD12;

                map pos = 1ull << sq;
                if (pos & queens) {
                    while (move) {
                        const bit to = PopBit(move);
                        //Callback_Move::template Queenmove<status, depth>(brd, pos, to);
                        moves.emplace_back(IsWhite, pos, to, encode_flags(0b0000, BoardPiece::bp_Queen, 0));
                    }
                }
                else {
                    while (move) {
                        const bit to = PopBit(move);
                        //Callback_Move::template Bishopmove<status, depth>(brd, pos, to);
                        moves.emplace_back(IsWhite, pos, to, encode_flags(0b0000, BoardPiece::bp_Bishop, 0));
                    }
                }
            }
            Bitloop(bish_nopin) {
                const square sq = SquareOf(bish_nopin);
                map move = lookup::Bishop(sq, Occ) & movableSquare;
                while (move) {
                    const bit to = PopBit(move);
                    //Callback_Move::template Bishopmove<status, depth>(brd, 1ull << sq, to);
                    moves.emplace_back(IsWhite, 1ull << sq, to, encode_flags(0b0000, BoardPiece::bp_Bishop, 0));
                }
            }
        }

        //Rookmoves (and horiz pinned queens)
        {
            map rooks = Rooks<white>() & ~pinD12;

            map rook_pinned = (rooks | queens) & pinHV;
            map rook_nopin = rooks & ~pinHV;
            Bitloop(rook_pinned) {
                const square sq = SquareOf(rook_pinned);
                map move = lookup::Rook(sq, Occ) & movableSquare & pinHV;
                map pos = 1ull << sq;
                if (pos & queens) {
                    while (move) {
                        const bit to = PopBit(move);
                        //Callback_Move::template Queenmove<status, depth>(brd, pos, to);
                        moves.emplace_back(IsWhite, pos, to, encode_flags(0b0000, BoardPiece::bp_Queen, 0));
                    }
                }
                else {
                    while (move) {
                        const bit to = PopBit(move);
                        //Callback_Move::template Rookmove<status, depth>(brd, pos, to);
                        moves.emplace_back(IsWhite, pos, to, encode_flags(0b0000, BoardPiece::bp_Rook, 0));
                    }
                }
            }
            Bitloop(rook_nopin) {
                    const square sq = SquareOf(rook_nopin);
                    map move = lookup::Rook(sq, Occ) & movableSquare;
                while (move) {
                    const bit to = PopBit(move);
                    //Callback_Move::template Rookmove<status, depth>(brd, 1ull << sq, to);
                    moves.emplace_back(IsWhite, 1ull << sq, to, encode_flags(0b0000, BoardPiece::bp_Rook, 0));
                }
            }
        }

        //Queen moves
        {
            map queens = Queens<white>() & ~(pinHV | pinD12);
            Bitloop(queens) {
                const square sq = SquareOf(queens);
                map atk = lookup::Queen(sq, Occ);
                map move = atk & movableSquare;
                while (move) {
                    const bit to = PopBit(move);
                    //Callback_Move::template Queenmove<status, depth>(brd, 1ull << sq, to);
                    moves.emplace_back(IsWhite, 1ull << sq, to, encode_flags(0b0000, BoardPiece::bp_Queen, 0));
                }
            }
        }
        //KingMoves (put last => lower priority when going over all the moves, other moves are probably better and can cause an earlier cut off
        {
            Bitloop(kingatk) {
                const square sq = SquareOf(kingatk);
                //Callback_Move::template Kingmove<status, depth>(brd, King<white>(brd), 1ull << sq);
                moves.emplace_back(IsWhite, King<IsWhite>(), 1ull << sq, encode_flags(0b0000, BoardPiece::bp_King, 0));
            }
        }

        return;
    }

    template<bool IsWhite>
    __forceinline void ClassicBitBoard::initCheckStatus()
    {
        constexpr bool white = IsWhite;
        constexpr bool enemy = !IsWhite;

        //Calculate check from enemy pawns
        const map pl = Pawn_AttackLeft<enemy>(Pawns<enemy>() & Pawns_NotLeft());
        const map pr = Pawn_AttackRight<enemy>(Pawns<enemy>() & Pawns_NotRight());
        if (pl & King<white>()) checkStatus = Pawn_AttackRight<white>(King<white>());
        else if (pr & King<white>()) checkStatus = Pawn_AttackLeft<white>(King<white>());
        else checkStatus = 0xffffffffffffffffull;

        //Calculate Check from enemy knights
        map knightcheck = lookup::Knight(SquareOf(King<white>()))& Knights<enemy>();
        if (knightcheck) checkStatus = knightcheck;
    }

    template<bool IsWhite>
    void ClassicBitBoard::RegisterPinD12(square king, square enemy) {
        const map pinMask = PinBetween[king * 64 + enemy];
        if (state.hasEP) {
            if (pinMask & EnPassantTarget) { 
                EnPassantTarget = 0; }
        }
        if (pinMask & OwnColor<IsWhite>()) { bishopPin |= pinMask; }
    }

    template<bool IsWhite>
    void ClassicBitBoard::RegisterPinHV(square king, square enemy)
    {
        const map pinMask = PinBetween[king * 64 + enemy];
        if (pinMask & OwnColor<IsWhite>()) { rookPin |= pinMask; }
    }
    template<bool IsWhite>
    _InlineConstExpr map ClassicBitBoard::EPRank()
    {
        if constexpr (IsWhite) return 0xFFull << 32;
        else return 0xFFull << 24;
    }
    template<bool IsWhite>
    __forceinline void ClassicBitBoard::RegisterPinEP(square kingsquare, bit king, map enemyRQ)
    {
        constexpr bool white = IsWhite;
        const map pawns = Pawns<IsWhite>();

        if ((EPRank<white>() & king) && (EPRank<white>() & enemyRQ) && (EPRank<white>() & pawns)) {
            bit EPLpawn = pawns & ((EnPassantTarget & Pawns_NotRight()) >> 1); //Pawn that can EPTake to the left 
            bit EPRpawn = pawns & ((EnPassantTarget & Pawns_NotLeft()) << 1);  //Pawn that can EPTake to the right
            
            if (EPLpawn) {
                map AfterEPocc = Occ & ~(EnPassantTarget | EPLpawn);
                if ((lookup::Rook(kingsquare, AfterEPocc) & EPRank <white>()) & enemyRQ) { EnPassantTarget = 0; }
            }
            if (EPRpawn) {
                map AfterEPocc = Occ & ~(EnPassantTarget | EPRpawn);
                if ((lookup::Rook(kingsquare, AfterEPocc) & EPRank<white>()) & enemyRQ) { EnPassantTarget = 0; }
            }
        }
    }
    void ClassicBitBoard::checkBySlider(square king, square enemy, map& kingban, map& checkmask)
    {
        if (checkmask == 0xffffffffffffffffull) {
            checkmask = PinBetween[king * 64 + enemy];
        }
        else { checkmask = 0; }
        kingban |= CheckBetween[king * 64 + enemy];
    }
    template<bool IsWhite>
    map ClassicBitBoard::Refresh(map& kingban, map& checkmask)
    {
        constexpr bool white = IsWhite;
        constexpr bool enemy = !IsWhite;
        const bit king = King<white>();
        const map kingsq = SquareOf(king);
        
        //Pinned pieces 9 checks by sliders
        {
            rookPin = 0; 
            bishopPin = 0;
     
            if (RookMask[kingsq] & EnemyRookQueen<white>()) {
                map atkHV = lookup::Rook(kingsq, Occ) & EnemyRookQueen<white>();
                Bitloop(atkHV) {
                    square sq = SquareOf(atkHV);
                    checkBySlider(kingsq, sq, kingban, checkmask);
                }
                map pinnersHV = lookup::Rook_Xray(kingsq, Occ) & EnemyRookQueen<white>();
                Bitloop(pinnersHV) {
                    RegisterPinHV<white>(kingsq, SquareOf(pinnersHV));
                }
            }
            if (BishopMask[kingsq] & EnemyBishopQueen<white>()) {
                map atkD12 = lookup::Bishop(kingsq, Occ) & EnemyBishopQueen<white>();
                Bitloop(atkD12) {
                    square sq = SquareOf(atkD12);
                    checkBySlider(kingsq, sq, kingban, checkmask);
                }
                map pinnersD12 = lookup::Bishop_Xray(kingsq, Occ) & EnemyBishopQueen<white>();
                Bitloop(pinnersD12) {
                    RegisterPinD12<white>(kingsq, SquareOf(pinnersD12));
                }                
            }
            if (state.hasEP) {
                RegisterPinEP<white>(kingsq, king, EnemyRookQueen<white>());
            }
        }
        //Movelist hpp line 31  Atk_king[depth] = King(SquareOf(King<white>()));
        map king_atk = lookup::King(SquareOf(King<white>())) & EnemyOrEmpty<white>() & ~kingban;
        if (king_atk == 0) return 0;

        //Calculate enemy knight
        {
            map knights = Knights<enemy>();
            Bitloop(knights) {
                kingban |= lookup::Knight(SquareOf(knights));
            }
        }


        //Calculate check form enemy pawns
        {
            const map pl = Pawn_AttackLeft<enemy>(Pawns<enemy>() & Pawns_NotLeft());
            const map pr = Pawn_AttackRight<enemy>(Pawns<enemy>() & Pawns_NotRight());

            kingban |= (pl | pr);
        }

        //calculate check form enemy bishop
        {
            map bishops = BishopQueen<enemy>();
            Bitloop(bishops) {
                const square sq = SquareOf(bishops);
                map atk = lookup::Bishop(sq, Occ);
                kingban |= atk;
            }
        }

        //calculate check form enemy rook
        {
            map rooks = RookQueen<enemy>();
            Bitloop(rooks) {
                const square sq = SquareOf(rooks);
                map atk = lookup::Rook(sq, Occ);
                kingban |= atk;
            }
        }

        return king_atk & ~kingban;
    }
    template<bool IsWhite>
    void ClassicBitBoard::Pawn_PruneLeft(map& pawn, const map pinD1D2)
    {
        const map pinned = pawn & Pawn_InvertLeft<IsWhite>(pinD1D2 & Pawns_NotRight()); //You can go left and are pinned
        const map unpinned = pawn & ~pinD1D2;

        pawn = (pinned | unpinned); //You can go left and you and your targetsquare is allowed
    }
    template<bool IsWhite>
    void chess::ClassicBitBoard::Pawn_PruneRight(map& pawn, const map pinD1D2)
    {
        const map pinned = pawn & Pawn_InvertRight<IsWhite>(pinD1D2 & Pawns_NotLeft()); //You can go right and are pinned
        const map unpinned = pawn & ~pinD1D2;

        pawn = (pinned | unpinned); //You can go right and you and your targetsquare is allowed
    }
    template<bool IsWhite>
    void chess::ClassicBitBoard::Pawn_PruneLeftEP(map& pawn, const map pinD1D2)
    {
        const map pinned = pawn & Pawn_InvertLeft<IsWhite>(pinD1D2 & Pawns_NotRight()); //You can go left and are pinned
        const map unpinned = pawn & ~pinD1D2;

        pawn = (pinned | unpinned);
    }

    template<bool IsWhite>
    void chess::ClassicBitBoard::Pawn_PruneRightEP(map& pawn, const map pinD1D2)
    {
        const map pinned = pawn & Pawn_InvertRight<IsWhite>(pinD1D2 & Pawns_NotLeft()); //You can go right and are pinned
        const map unpinned = pawn & ~pinD1D2;

        pawn = (pinned | unpinned);
    }
    template<bool IsWhite>
    void chess::ClassicBitBoard::Pawn_PruneMove(map& pawn, const map pinHV)
    {
        const map pinned = pawn & Pawn_Backward<IsWhite>(pinHV); //You can forward and are pinned by rook/queen in forward direction
        const map unpinned = pawn & ~pinHV;

        pawn = (pinned | unpinned); //You can go forward and you and your targetsquare is allowed
    }
    template<bool IsWhite>
    void chess::ClassicBitBoard::Pawn_PruneMove2(map& pawn, const map pinHV)
    {
        const map pinned = pawn & Pawn_Backward2<IsWhite>(pinHV); //You can forward and are pinned by rook/queen in forward direction
        const map unpinned = pawn & ~pinHV;

        pawn = (pinned | unpinned); //You can go forward and you and your targetsquare is allowed
    }

    template<bool InCheck>
    _InlineConstExpr map chess::ClassicBitBoard::ConstCheckmask(const map checkmask)
    {
        if constexpr (InCheck) return checkmask;
        else return 0xffffffffffffffffull;
    }

    template<bool white, bool InCheck>
    _InlineConstExpr map chess::ClassicBitBoard::MoveableSquares(const map checkmask)
    {
        if constexpr (InCheck) return EnemyOrEmpty<white>() & checkmask;
        else return EnemyOrEmpty<white>();
    }

#pragma endregion

    
    /*******************************\
    
            CLASS MOVEHISTORY
    
    \*******************************/
    #pragma region History
    #pragma endregion

    /**************************\

            CLASS MOVE

    \**************************/
    #pragma region Move
    Move::Move() : IsWhite(true), from(0), to(0), flags(0) {}
    Move::Move(bool IsWhite, bit from, bit to, Flags flags) : IsWhite(IsWhite), from(from),to(to), flags(flags) {}
    std::string chess::Move::toLAN()
    {
        if (this->from == this->to) return "";
        square sqf = SquareOf(this->from);
        square sqt = SquareOf(this->to);
        char ff = "hgfedcba"[SquareOf(this->from)%8];
        char rf = "12345678"[SquareOf(this->from)/8];
        char ft = "hgfedcba"[SquareOf(this->to)%8];
        char rt = "12345678"[SquareOf(this->to)/8];
        std::string lan = "";
        lan += ff;
        lan += rf;
        lan += ft;
        lan += rt;
        int promote = getMovePromote(this->flags);
        switch (promote) {
        case BoardPiece::bp_Queen:
            return lan + "q";
        case BoardPiece::bp_Bishop:
            return lan + "b";
        case BoardPiece::bp_Knight:
            return lan + "n";
        case BoardPiece::bp_Rook:
            return lan + "r";
        default:
            return lan;

        }
        return lan;
    }
    std::string chess::Move::toLAN(const Move& move)
    {
        if (move.from == move.to) return "";
        square sqf = SquareOf(move.from);
        square sqt = SquareOf(move.to);
        if (sqf == sqt) { return ""; }
        char ff = "hgfedcba"[SquareOf(move.from) % 8];
        char rf = "12345678"[SquareOf(move.from) / 8];
        char ft = "hgfedcba"[SquareOf(move.to) % 8];
        char rt = "12345678"[SquareOf(move.to) / 8];
        std::string lan = "";
        lan += ff;
        lan += rf;
        lan += ft;
        lan += rt;
        int promote = getMovePromote(move.flags);
        switch (promote) {
        case BoardPiece::bp_Queen:
            return lan + "q";
        case BoardPiece::bp_Bishop:
            return lan + "b";
        case BoardPiece::bp_Knight:
            return lan + "n";
        case BoardPiece::bp_Rook:
            return lan + "r";
        default:
            return lan;
        }
        return lan;
    }

    void chess::Move::compress(Move &move,uint32_t &compressed)
    {
        compressed = 0;
        compressed ^= move.flags;       //Lowest half;
        compressed ^= move.IsWhite << 31; //MSB
        compressed ^= (uint8_t)(SquareOf(move.from)) << 16;
        compressed ^= (uint8_t)(SquareOf(move.to))   << 24; //MAx 64 => MSB never written
    }

    void chess::Move::decompress(uint32_t& compressed, Move& move) {
        move.flags = compressed & 0xFFFF;
        move.from = 1ull << ((compressed >> 16) & 0xFF);
        move.to   = 1ull << ((compressed >> 24) & 0x7F);
        move.IsWhite = compressed >> 31;
    }

    bool operator==(const Move& move1, const Move& move2)
    {
        if (move1.IsWhite != move2.IsWhite) return false;
        if (move1.from != move2.from) return false;
        if (move1.to != move2.to) return false;
        if (move1.flags != move2.flags) return false;
        return true;
    }


#pragma endregion

    /**************************\

            CLASS HASH

    \**************************/
    #pragma region Hash
    uint64_t ClassicBitBoard::HashUtil::createHash(ClassicBitBoard& board)
    {
        uint64_t hash = 0ull;
        //Loop over every piece
        map piece = board.WPawn;
        while(piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Pawn, true>(SquareOf(sq));
        }
        piece = board.BPawn;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Pawn, false>(SquareOf(sq));
        }
        piece = board.WKnight;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Knight, true>(SquareOf(sq));
        }
        piece = board.BKnight;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Knight, false>(SquareOf(sq));
        }
        piece = board.WBishop;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Bishop, true>(SquareOf(sq));
        }
        piece = board.BBishop;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Bishop, false>(SquareOf(sq));
        }
        piece = board.WRook;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Rook, true>(SquareOf(sq));
        }
        piece = board.BRook;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Rook, false>(SquareOf(sq));
        }
        piece = board.WQueen;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Queen, true>(SquareOf(sq));
        }
        piece = board.BQueen;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_Queen, false>(SquareOf(sq));
        }
        piece = board.WKing;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_King, true>(SquareOf(sq));
        }
        piece = board.BKing;
        while (piece) {
            const bit sq = PopBit(piece);
            hash ^= HashUtil::getPieceHash<bp_King, false>(SquareOf(sq));
        }

        //Castling:
        if (board.state.WCastleL) hash ^= HashUtil::hashWCastleL();
        if (board.state.WCastleR) hash ^= HashUtil::hashWCastleS(); 
        if (board.state.BCastleL) hash ^= HashUtil::hashBCastleL();
        if (board.state.BCastleR) hash ^= HashUtil::hashBCastleS();

        //enpassant
        if (board.state.hasEP) {
            //Note: EPtarget = square of pawn that can be caputerd but this is the same file needed to compute hash as the ep square
            hash ^= HashUtil::hashEnPassant(board.side,board.EnPassantTarget, board.WPawn, board.BPawn);
        }

        //turn
        if (board.side) hash ^= HashUtil::turn();

        return hash;
    }
    #pragma endregion

    void ClassicBitBoard::BoardState::Reset()
    {
        hasEP = false;
        BCastleL = true;
        BCastleR = true;
        WCastleL = true;
        WCastleR = true;
    }
    template <bool IsWhite>
    _InlineConstExpr bool chess::ClassicBitBoard::BoardState::canCastleLeft(map attacked, map occupied, map rook)
    {
        if constexpr (IsWhite) {
            if (WCastleL) {
                if (occupied & WNotOccupiedL) return false;
                if (attacked & WNotAttackedL) return false;
                if (rook & WRookL) return true;
                return false;
            }
        }
        else {
            if (BCastleL) {
                if (occupied & BNotOccupiedL) return false;
                if (attacked & BNotAttackedL) return false;
                if (rook & BRookL) return true;
                return false;
            }
        }
        return false;
    }
    template <bool IsWhite>
    _InlineConstExpr bool chess::ClassicBitBoard::BoardState::canCastleRight(map attacked, map occupied, map rook)
    {
        if constexpr (IsWhite) {
            if (WCastleR) {
                if (occupied & WNotOccupiedR) return false;
                if (attacked & WNotAttackedR) return false;
                if (rook & WRookR) return true;
                return false;
            }
        }
        else {
            if (BCastleR) {
                if (occupied & BNotOccupiedR) return false;
                if (attacked & BNotAttackedR) return false;
                if (rook & BRookR) return true;
                return false;
            }
        }
        return false;
    }

    ClassicBitBoard::History::History(const ClassicBitBoard &brd)
    {
        this->BPawn = brd.BPawn;
        this->BKnight = brd.BKnight;
        this->BBishop = brd.BBishop;
        this->BRook = brd.BRook;
        this->BQueen = brd.BQueen;
        this->BKing = brd.BKing;
        this->WPawn = brd.WPawn;
        this->WKnight = brd.WKnight;
        this->WBishop = brd.WBishop;
        this->WRook = brd.WRook;
        this->WQueen = brd.WQueen;
        this->WKing = brd.WKing;

        this->Black = brd.Black;
        this->White = brd.White;
        this->Occ = brd.Occ;
        this->EnPassantTarget = brd.EnPassantTarget;
        this->rookPin = brd.rookPin;
        this->bishopPin = brd.bishopPin;
        this->halfMoves = brd.halfmoves;
        this->hasEP = brd.state.hasEP;
        this->BCastleL = brd.state.BCastleL;
        this->BCastleR = brd.state.BCastleR;
        this->WCastleL = brd.state.WCastleL;
        this->WCastleR = brd.state.WCastleR;
    }

    ClassicBitBoard::History::History(const History& h)
    {

        this->BPawn = h.BPawn;
        this->BKnight = h.BKnight;
        this->BBishop = h.BBishop;
        this->BRook = h.BRook;
        this->BQueen = h.BQueen;
        this->BKing = h.BKing;
        this->WPawn = h.WPawn;
        this->WKnight = h.WKnight;
        this->WBishop = h.WBishop;
        this->WRook = h.WRook;
        this->WQueen = h.WQueen;
        this->WKing = h.WKing;

        this->Black = h.Black;
        this->White = h.White;
        this->Occ = h.Occ;
        this->EnPassantTarget = h.EnPassantTarget;
        this->rookPin = h.rookPin;
        this->bishopPin = h.bishopPin;
        this->halfMoves = h.halfMoves;
        this->hasEP = h.hasEP;
        this->BCastleL = h.BCastleL;
        this->BCastleR = h.BCastleR;
        this->WCastleL = h.WCastleL;
        this->WCastleR = h.WCastleR;
    }
  


}