
#ifndef UAFTI_CHESSAI_CHESSAITRAINER_H
#define UAFTI_CHESSAI_CHESSAITRAINER_H
#include <torch/torch.h>
#include "ChessDataset.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>


struct ChessAIOptions{
    int epochs = 30;
    int trainBatchSize = 64;
    int validationBatchSize = 256;

    int workers = 6;
    int jobs = 8;

    double lr = 2e-3;

    torch::Device device = torch::kCUDA;

};
class ChessAITrainer {
public:
    ChessDataset training = ChessDataset();
    ChessDataset validation = ChessDataset();
    std::vector<uint64_t> trainingLoss;
    std::vector<uint64_t> validationLoss;
    torch::data::DataLoaderOptions trainOptions;
    torch::data::DataLoaderOptions validationOptions;
    ChessAIOptions options;

    //std::shared_ptr<ChessAIModel> model;
    ChessAIModel model;

    ChessAITrainer();
    ChessAITrainer(ChessDataset& data, ChessAIOptions& options);
    void start();
    void train(int epoch);
    void validate(int epoch);
    void save(int epoch);
    std::shared_ptr<ChessAIModel> load(std::string file);
    std::string getTime();
    };


#endif //UAFTI_CHESSAI_CHESSAITRAINER_H
