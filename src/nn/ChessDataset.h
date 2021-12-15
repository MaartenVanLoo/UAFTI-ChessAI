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
    std::string path,fenFile, evalFile;
public:
    std::vector<std::string> fen;
    std::vector<int> eval;

    ChessDataset(){

    }
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
        while (std::getline(fenstream,fenData)){
            evalstream >> evalData;

            //parse eval value:
            if (evalData.find("(m)") != std::string::npos){
                //discard mate in values
                continue;
            }
            else if (evalData.find("(c)") != std::string::npos) {
                evalData = evalData.substr(4);
            }
            double value = std::stod(evalData);
            fenData.shrink_to_fit(); //Avoid to much memory reserved for string;
            this->fen.push_back(fenData);
            this->eval.push_back((int)(value*100));
        }
        this->fen.shrink_to_fit();
        this->eval.shrink_to_fit();
    }

    ExampleType get(size_t index) override {
        //std::cout << this->fen[index]<<std::endl;
        chess::ClassicBitBoard board = chess::ClassicBitBoard(this->fen[index]);
        torch::Tensor input = ChessAIModel::boardToTensor(board);
        torch::Tensor output = torch::zeros(1);
        output[0] = this->eval[index];
        return {input,output};
    }

    torch::optional<size_t> size() const override {
        return fen.size();
    }

    void split(ChessDataset& training, ChessDataset& validation,double validationPart){
        size_t validationSize = this->fen.size()*validationPart;
        size_t trainingSize = this->fen.size()-validationSize;

        torch::data::samplers::RandomSampler random(this->fen.size());
        std::vector<size_t> validationIndex = random.next(validationSize).value();
        std::sort(validationIndex.begin(),validationIndex.end());

        training.fen.reserve(trainingSize);
        training.eval.reserve(trainingSize);
        validation.fen.reserve(validationSize);
        validation.eval.reserve(validationSize);

        auto validationIterator = validationIndex.begin();
        for (int i = 0; i < this->fen.size(); i++){
            if (*validationIterator == i){
                validation.fen.push_back(this->fen[i]);
                validation.eval.push_back(this->eval[i]);
                validationIterator++;
            }else{
                training.fen.push_back(this->fen[i]);
                training.eval.push_back(this->eval[i]);
            }
        }
        //When done => clear this fen and eval;
        this->fen.clear();
        this->fen.shrink_to_fit();
        this->eval.clear();
        this->eval.shrink_to_fit();
    }
};

#endif //UAFTI_CHESSAI_CHESSDATASET_H
