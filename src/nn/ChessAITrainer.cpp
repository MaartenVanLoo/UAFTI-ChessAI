//
// Created by maart on 13/12/2021.
//

#include "ChessAITrainer.h"

ChessAITrainer::ChessAITrainer() {
    this->training = ChessDataset();
    this->validation = ChessDataset();
}
ChessAITrainer::ChessAITrainer(ChessDataset& data,ChessAIOptions& options) :options(options){
    data.split(this->training,this->validation, 0.2);
}

void ChessAITrainer::start(){
    if (options.workers == 0){
        options.jobs = 0;
    }
    //Output settings
    std::cout << "Training starting: \n";
    std::cout << "  Workers:" << options.workers << "\n";
    std::cout << "  Jobs: " << options.jobs << "\n";
    std::cout << "  Device: " << options.device << "\n";
    std::cout << "  Training Options:\n";
    std::cout << "    Batch size   :" << options.trainBatchSize << "\n";
    std::cout << "    learning rate:" << options.lr << "\n";
    std::cout << "  Validation Options:\n";
    std::cout << "    Batch size   :" << options.validationBatchSize << "\n";

    //init dataloaders
    trainOptions = torch::data::DataLoaderOptions()
                            .batch_size(this->options.trainBatchSize)
                            .workers(this->options.workers)
                            .max_jobs(this->options.jobs);
    validationOptions = torch::data::DataLoaderOptions()
                            .batch_size(this->options.validationBatchSize)
                            .workers(this->options.workers)
                            .max_jobs(this->options.jobs);



    //move model to training device
    this->model.to(options.device);

    for (int epoch =0; epoch< this->options.epochs; epoch++){
        this->model.train(true);
        std::cout << "Model Is training: " << this->model.is_training() << std::endl;
        train(epoch);

        this->model.train(false);
        std::cout << "Model Is training: " << this->model.is_training() << std::endl;
        validate(epoch);
        save(epoch); // Serialize your model periodically as a checkpoint.

    }
}
void ChessAITrainer::train(int epoch) {
    torch::optim::Adam optimizer(this->model.parameters(), torch::optim::AdamOptions(this->options.lr));
    //torch::nn::MSELoss criterion = torch::nn::MSELoss();
    torch::nn::SmoothL1Loss criterion = torch::nn::SmoothL1Loss();
    auto trainingLoader = torch::data::make_data_loader(this->training.map(torch::data::transforms::Stack<>()), trainOptions);

    //usefull variables:
    size_t batchCount = (*trainingLoader).options().drop_last?
            this->training.size().value()/options.trainBatchSize:
            this->training.size().value()/options.trainBatchSize + 1;


    float running_loss = 0.0;
    int batch_index = 0;
    for (auto &batch : *trainingLoader) {
        torch::Tensor board = batch.data.to(this->options.device);
        torch::Tensor eval = batch.target.to(this->options.device);

        optimizer.zero_grad();

        torch::Tensor output = this->model.forward(board);
        torch::Tensor loss = criterion(output,eval);
        loss.backward();
        optimizer.step();

        running_loss+=loss.item<float>();
        if (++batch_index % 200 == 0) {
            std::cout << "[" << getTime() << "] ";
            std::cout << "Epoch: [" << (epoch+1) << "/" << this->options.epochs;
            std::cout <<"] | Batch: [" << batch_index
                      << "/" << batchCount;
            std::cout <<"] | Running Loss: " << running_loss/(float)batch_index << std::endl;
        }
    }
    trainingLoss.push_back(running_loss/(float)batch_index);
    std::cout << "[" << getTime() << "] ";
    std::cout << "Epoch: [" << (epoch+1) << "/" << this->options.epochs;
    std::cout <<"] | Batch: [" << batch_index
              << "/" << batchCount;
    std::cout <<"] | Running Loss: " << running_loss/(float)batch_index << std::endl;
}

void ChessAITrainer::validate(int epoch) {
    auto validationLoader = torch::data::make_data_loader(this->validation.map(torch::data::transforms::Stack<>()), validationOptions);
    //torch::nn::MSELoss criterion = torch::nn::MSELoss();
    torch::nn::L1Loss criterionL1 = torch::nn::L1Loss();
    torch::nn::SmoothL1Loss criterion = torch::nn::SmoothL1Loss();

    torch::NoGradGuard no_grad; //No gradient for validation$

    //usefull variables:
    size_t batchCount = (*validationLoader).options().drop_last?
                        this->validation.size().value()/options.validationBatchSize:
                        this->validation.size().value()/options.validationBatchSize + 1;

    float running_loss = 0.0;
    float running_L1loss = 0.0;
    int batch_index = 0;
    torch::Tensor maxDeviation = torch::tensor(1);
    for (auto &batch : *validationLoader) {

        torch::Tensor board = batch.data.to(this->options.device);
        torch::Tensor eval = batch.target.to(this->options.device);
        torch::Tensor output = this->model.forward(board);
        torch::Tensor loss = criterion(output,eval);
        torch::Tensor lossL1 = criterionL1(output,eval);

        running_loss+=loss.item<float>();
        running_L1loss+=lossL1.item<float>();
        torch::Tensor diff = torch::abs(eval-output);
        //std::cout << batch_index << ":" << torch::max(diff);

        //find maximum deviation as wrong color 'advatage'
        torch::Tensor mask = torch::ne(torch::sign(eval), torch::sign(output));
        torch::Tensor maxVal = torch::max(torch::masked_select(diff,mask));

        if (maxVal.numel()>0){
            maxDeviation = torch::max(maxDeviation,maxVal); //compute maximum deviation
            if (maxVal.numel()>0 &&  maxVal.item<float>() == maxDeviation.item<float>()){
                mask = torch::eq(maxVal,diff);

                std::cout << "Expected value: " << torch::masked_select(eval, mask) << std::endl;
                std::cout << "Actual value  : " << torch::masked_select(output, mask) << std::endl;

            }
        }


        if (++batch_index % 200 == 0) {
            std::cout << "[" << getTime() << "] ";
            std::cout << "Validation: [" << (epoch+1) << "/" << this->options.epochs;
            std::cout <<"] | Batch: [" << batch_index
                      << "/" << batchCount;
            std::cout <<"] | Running Loss: " << std::setw(7) <<running_loss/(float)batch_index;
            std::cout <<" | L1: " << std::setw(7) <<running_L1loss/(float)batch_index;
            std::cout <<" | Max dev: "<< maxDeviation.item<float>() << std::endl;

        }
    }
    validationLoss.push_back(running_loss/(float)batch_index);
    std::cout << "[" << getTime() << "] ";
    std::cout << "Validation: [" << (epoch+1) << "/" << this->options.epochs;
    std::cout <<"] | Batch: [" << batch_index
              << "/" << batchCount;
    std::cout <<"] | Running Loss: " << std::setw(7) <<running_loss/(float)batch_index;
    std::cout <<" | L1: " << std::setw(7) <<running_L1loss/(float)batch_index;
    std::cout <<" | Max dev: "<< maxDeviation.item<float>() << std::endl;
    chess::ClassicBitBoard board = chess::ClassicBitBoard(chess::ClassicBitBoard::startpos);
    torch::Tensor start = ChessAIModel::boardToTensor(board).to(options.device);;
    start.reshape({1,-1});
    torch::Tensor startpos = this->model.forward(start);
    std::cout << "Startpos evaluation: " << startpos.item<float>() << std::endl;
}

void ChessAITrainer::save(int epoch) {
    //Give training overview:
    for (int i = 0; i < validationLoss.size(); i++){
        std::cout << "Epoch: " << (i+1) << " |\t"
                  << "Training loss: " << trainingLoss[i] << " |\t"
                  << "Validation loss: " << validationLoss[i]
                  << std::endl;
    }
    std::ostringstream oss;
    oss << "model" << std::setw(2) << std::setfill('0') << (epoch+1);
    std::string filename = oss.str() + ".param";
    this->model.saveParameters(filename);

    std::shared_ptr<ChessAIModel> ptr = std::make_shared<ChessAIModel>(this->model);
    filename = oss.str() + ".pt";
    torch::save(ptr, filename);
}

std::shared_ptr<ChessAIModel> ChessAITrainer::load(std::string file){
    std::ifstream f(file.c_str());
    if (f.good()) {
        std::cout << "file exists" << std::endl;
    }
    f.close();
    std::shared_ptr<ChessAIModel> ptr = std::make_shared<ChessAIModel>();
    std::cout << "made shared" << std::endl;
    torch::load(ptr, file);
    std::cout << "loaded model" << std::endl;
    return ptr;
}

std::string ChessAITrainer::getTime() {
    using namespace std::chrono;

    // get current time
    auto now = system_clock::now();

    // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = system_clock::to_time_t(now);

    // convert to broken time
    std::tm bt;
    localtime_s(&bt, &timer);

    std::ostringstream oss;

    oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}



