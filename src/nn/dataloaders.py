import concurrent.futures
import linecache
import string
from builtins import int, bool
from concurrent.futures import ProcessPoolExecutor
from typing import Tuple, Any

import torch
from torch.utils.data import Dataset
import PyChess as chess
import asyncio

class Options:
    def __init__(self):
        self.random_seed = 1
        self.device = "cuda:0"
        self.train_dataset_size = 128
        self.test_dataset_size = 128
        self.num_epochs = 1

class ChessDataset(Dataset):

    def __init__(self, path: string = "", fileIndex: int = 0,options: Options = Options(), train : bool = True):
        super()
        self.options = options
        if train:
            self.dataset_size = options.train_dataset_size
        else:
            self.dataset_size = options.test_dataset_size
        self.f_fen = f"{path}/FEN/FenBatch{fileIndex:02}.txt"
        self.f_eval = f"{path}/EVAL/FenBatch{fileIndex:02}Eval.txt"
        f = open(self.f_fen,'r')
        self.fen = f.readlines()
        f.close()
        f = open(self.f_eval, 'r')
        self.eval = f.readlines()
        f.close()

        #get count:
        """self.length = 0
        f=open(self.f_fen)
        while f.readline():
            self.length+=1"""
        self.length = len(self.eval)

    def __len__(self):
        return self.length

    def __getitem__(self,idx)  -> Tuple[Any, Any]:
        if torch.is_tensor(idx):
            idx = idx.tolist()
        """Convert FEN to tensor"""
        board = chess.Board()
        #input = torch.ShortTensor([1,780])
        #for id in range(len(idx)):
        #fen = linecache.getline(self.f_fen,idx+1)
        #print(fen)
        #linecache.clearcache()
        board.set_fen(self.fen[idx])
        input = self.boardToTensor(board)


        """get eval value"""
        if (self.eval[idx][0] == '('):
            if ("(m)" in self.eval[idx]):
                self.eval[idx] = "0" #WARNING => PLEASE REMOVE ALREADY IN GENEATION OF DATA!
            elif("(c" in self.eval[idx]):
                self.eval[idx] = self.eval[idx][4::]
        output = torch.Tensor([int(float(self.eval[idx])*100)])
        """Cat 2 """
        return (input,output)

    def boardToTensor(self,board)->torch.Tensor:
        # BLACK PAWNS       [0  :63 ]
        # BLACK ROOKS       [64 :127]
        # BLACK BISHOPS     [128:191]
        # BLACK KNIGHTS     [192:255]
        # BLACK QUEENS      [256:319]
        # BLACK KINGS       [320:383]
        # WHITE PAWNS       [384:447]
        # WHITE ROOKS       [448:511]
        # WHITE BISHOPS     [512:575]
        # WHITE KNIGHTS     [576:639]
        # WHITE QUEENS      [640:702]
        # WHITE KINGS       [704:767]
        # Castling B_Left   [768]
        # Castling B_Right  [769]
        # Castling W_Left   [770]
        # Castling W_Right  [771]
        # ep squares a->h   [772,779]
        # halfmoves         [780]
        # fullmoves         [781]
        tensor = torch.zeros(784)

        blackPawns = board.pawns & board.occupied_co[chess.BLACK]
        blackRooks = board.rooks & board.occupied_co[chess.BLACK]
        blackBishops = board.bishops & board.occupied_co[chess.BLACK]
        blackKnights = board.knights & board.occupied_co[chess.BLACK]
        blackQueens = board.queens & board.occupied_co[chess.BLACK]
        blackKings = board.kings & board.occupied_co[chess.BLACK]
        whitePawns = board.pawns & board.occupied_co[chess.WHITE]
        whiteRooks = board.rooks & board.occupied_co[chess.WHITE]
        whiteBishops = board.bishops & board.occupied_co[chess.WHITE]
        whiteKnights = board.knights & board.occupied_co[chess.WHITE]
        whiteQueens = board.queens & board.occupied_co[chess.WHITE]
        whiteKings = board.kings & board.occupied_co[chess.WHITE]

        # blackpawns:
        while blackPawns:
            sq = self.SquareOf(blackPawns)
            blackPawns ^= 1 << sq
            tensor[0+sq] = 1
        # blackRooks:
        while blackRooks:
            sq = self.SquareOf(blackRooks)
            blackRooks ^= 1 << sq
            tensor[64+sq] = 1
        # blackBishops:
        while blackBishops:
            sq = self.SquareOf(blackBishops)
            blackBishops ^= 1 << sq
            tensor[128+sq] = 1
        # blackKnights:
        while blackKnights:
            sq = self.SquareOf(blackKnights)
            blackKnights ^= 1 << sq
            tensor[192+sq] = 1
        # blackQueens:
        while blackQueens:
            sq = self.SquareOf(blackQueens)
            blackQueens ^= 1 << sq
            tensor[256+sq] = 1
        # blackKings:
        while blackKings:
            sq = self.SquareOf(blackKings)
            blackKings ^= 1 << sq
            tensor[320+sq] = 1
        # whitePawns:
        while whitePawns:
            sq = self.SquareOf(whitePawns)
            whitePawns ^= 1 << sq
            tensor[384+sq] = 1
        # whiteRooks:
        while whiteRooks:
            sq = self.SquareOf(whiteRooks)
            whiteRooks ^= 1 << sq
            tensor[448+sq] = 1
        # whiteBishops:
        while whiteBishops:
            sq = self.SquareOf(whiteBishops)
            whiteBishops ^= 1 << sq
            tensor[512+sq] = 1
        # whiteKnights:
        while whiteKnights:
            sq = self.SquareOf(whiteKnights)
            whiteKnights ^= 1 << sq
            tensor[576+sq] = 1
        # whiteQueens:
        while whiteQueens:
            sq = self.SquareOf(whiteQueens)
            whiteQueens ^= 1 << sq
            tensor[640+sq] = 1
        # whiteKings:
        while whiteKings:
            sq = self.SquareOf(whiteKings)
            whiteKings ^= 1 << sq
            tensor[704+sq] = 1

        #castling
        if (board.castling_rights & chess.BB_A8):
            tensor[768] = 1
        if (board.castling_rights & chess.BB_H8):
            tensor[769] = 1
        if (board.castling_rights & chess.BB_A1):
            tensor[770] = 1
        if (board.castling_rights & chess.BB_H1):
            tensor[771] = 1

        #ep
        if board.has_legal_en_passant():
            if board.ep_square & chess.BB_RANK_1:
                tensor[772] = 1
            if board.ep_square & chess.BB_RANK_2:
                tensor[773] = 1
            if board.ep_square & chess.BB_RANK_3:
                tensor[774] = 1
            if board.ep_square & chess.BB_RANK_4:
                tensor[775] = 1
            if board.ep_square & chess.BB_RANK_5:
                tensor[776] = 1
            if board.ep_square & chess.BB_RANK_6:
                tensor[777] = 1
            if board.ep_square & chess.BB_RANK_7:
                tensor[778] = 1
            if board.ep_square & chess.BB_RANK_8:
                tensor[779] = 1
        tensor[780] = board.halfmove_clock
        tensor[781] = board.fullmove_number
        if (board.turn == chess.WHITE):
            tensor[782] = 0
            tensor[783] = 1
        else:
            tensor[782] = 1
            tensor[783] = 0
        return tensor

    def SquareOf(self, bitmap):
        return self.countTrailingZero(bitmap)

    def countTrailingZero(self,x):
        count = 0
        while ((x & 1) == 0):
            x = x >> 1
            count += 1

        return count

