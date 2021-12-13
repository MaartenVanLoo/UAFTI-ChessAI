//
// Created by maart on 13/12/2021.
//

#ifndef UAFTI_CHESSAI_CHESSDATASET_H
#define UAFTI_CHESSAI_CHESSDATASET_H
#include <torch/torch.h>
#include <string>
#include <sstream>
#include "../chess/ClassicBitBoard.h"
#include "ChessAIModel.h"

class ChessDataset : public torch::data::Dataset<ChessDataset>{
private:
    std::vector<std::string> fen;
    std::vector<int> eval;
    std::string path,fenFile, evalFile;
public:
    ChessDataset(const std::string& path, int index) : path(path){
        std::ifstream fenstream;
        std::ifstream evalstream;

        std::stringstream fileIndex;
        fileIndex << std::setw(2) << std::setfill('0') << index;
        this->fenFile  = path + "/FEN/FenBatch"  + fileIndex.str() + ".txt";
        this->evalFile = path + "/EVAL/FenBatch" + fileIndex.str() + "Eval.txt";

        fenstream.open(this->fenFile);
        evalstream.open(this->evalFile);

        if (fenstream.fail()){
            std::cerr << "Failed to open FEN file" << std::endl;
            return;
        }
        if (evalstream.fail()){
            std::cerr << "Failed to open EVAL file" << std::endl;
            return;
        }
        //preallocate vectors to 2^22 (±4 milion) elements
        fen.reserve(1<<22);
        eval.reserve(1<<22);

        //load data:
        std::string fenData,evalData;
        while (fenstream >> fenData){
            evalstream >> evalData;

            //parse eval value:
            if (evalData.find("(m)") != std::string::npos){
                //discard mate in values
                continue;
            }
            else if (evalData.find("(c)") != std::string::npos) {
                evalData = evalData.substr(4);
            }
            this->fen.push_back(evalData);
            this->eval.push_back(std::stod(evalData));
        }
    }

    ExampleType get(size_t index) override {
        chess::ClassicBitBoard board = chess::ClassicBitBoard(this->fen[index]);
        torch::Tensor input = ChessAIModel::boardToTensor(board);
        torch::Tensor output = torch::zeros({1});
        output[0] = this->eval[index];
        return {input,output};
    }

    torch::optional<size_t> size() const override {
        return fen.size();
    }
};



#endif //UAFTI_CHESSAI_CHESSDATASET_H
