#include <torch/torch.h>
#include <iostream>
#include "evalModel.h"
#include "ChessDataset.h"
#include "ChessAITrainer.h"


void loadAndRandEval(){
    torch::Device device = torch::kCPU;
    // load model
    ChessAITrainer trainer =ChessAITrainer();
    std::string file = "model11.pt";
    std::shared_ptr<ChessAIModel> model = trainer.load(file);
    torch::Tensor random = torch::rand({784});
    random.reshape({1,-1});
    std::cout << random << std::endl;
    std::cout << (*model) << std::endl;
    model->to(device);
    random.to(device);
    torch::Tensor result = model->forward(random);
    std::cout << result << std::endl;
    //save random vector and result
    std::vector<torch::Tensor> vec = {
            random, result,
    };
    std::vector<char> chars = torch::pickle_save(vec);
    std::ofstream ofstream("random.tensor", std::ios::out | std::ios::binary);
    ofstream.write(chars.data(), chars.size());
    ofstream.close();
}
int main() {
    loadAndRandEval();

    torch::Tensor t1 = torch::rand({3})*2-1;
    torch::Tensor t2 = torch::rand({3})*2-1;
    torch::Tensor mask =  torch::logical_and(torch::not_equal(torch::sign(t1),torch::sign(t2)),
                                             torch::ge(torch::abs(t1),torch::abs(t2)));
    torch::Tensor t3 = torch::masked_select(torch::abs(t1-t2),mask);
    std::cout << t1 << std::endl;
    std::cout << t2 << std::endl;
    std::cout << mask << std::endl;
    std::cout << t3 << std::endl;


    ChessAIModel model = ChessAIModel();
    std::string f = "test";
    model.saveParameters(f);


    model.to(torch::kCPU);
    chess::ClassicBitBoard board = chess::ClassicBitBoard(chess::ClassicBitBoard::startpos);
    torch::Tensor b1 = ChessAIModel::boardToTensor(board).to(torch::kCPU);
    b1.reshape({1,-1});
    torch::Tensor startpos = model.forward(b1);
    std::cout << "Startpos evaluation: " << startpos.item<float>() << std::endl;

    auto t = ChessAIModel::boardToTensor(board);
    //std::cout << t << std::endl;
    double v = std::stod("+0.51");
    std::cout << v << std::endl;
    torch::Device device = torch::kCPU;
    std::cout << "CUDA DEVICE COUNT: " << torch::cuda::device_count() << std::endl;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }
    torch::Tensor tensor = torch::rand({2, 3});
    std::cout << tensor << std::endl;
    torch::Tensor tensor2 = torch::randn({3, 4}, torch::dtype(torch::kFloat32).device(torch::kCUDA).requires_grad(true));
    std::cout << tensor2 << std::endl;
    system("pause");
    // Create a new Net.
    auto net = std::make_shared<Eval>();

    // Create a multi-threaded data loader for the MNIST dataset.
    std::cout <<"./mnist" << std::endl;
    std::ofstream test;
    test.open("./mnist/test.txt");
    test.close();
    auto dataset = torch::data::datasets::MNIST("./mnist")
            .map(torch::data::transforms::Normalize<>(0.5, 0.5))
            .map(torch::data::transforms::Stack<>());
    std::cout << "dataset complete" << std::endl;
    auto data_loader = torch::data::make_data_loader(
            std::move(dataset),
            torch::data::DataLoaderOptions().batch_size(64).workers(8).max_jobs(4));
    std::cout << "dataloader complete" << std::endl;
    // Instantiate an SGD optimization algorithm to update our Net's parameters.
    /**
    for (torch::data::Example<>& batch : *data_loader) {
        std::cout << "Batch size: " << batch.data.size(0) << " | Labels: ";
        for (int64_t i = 0; i < batch.data.size(0); ++i) {
            std::cout << batch.target[i].item<int64_t>() << " ";
        }
        std::cout << std::endl;
    }**/
    Eval eval = Eval();
    eval.to(device);
    torch::optim::Adam optimizer(eval.parameters(), torch::optim::AdamOptions(2e-4));
    auto criterion = torch::nn::CrossEntropyLoss();

    auto start = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    for (int64_t epoch = 1; epoch <= 1; ++epoch) {
        int64_t batch_index = 0;
        for (torch::data::Example<> &batch : *data_loader) {
            optimizer.zero_grad();
            torch::Tensor images = batch.data.to(device);
            torch::Tensor labels = batch.target.to(device);
            torch::Tensor output = eval.forward(images);
            torch::Tensor loss = criterion(output,labels);
            loss.backward();
            optimizer.step();

            if (++batch_index % 100 == 0) {
                std::cout << "Epoch: " << epoch << " | Batch: " << batch_index
                          << " | Loss: " << loss.item<float>() << std::endl;
                // Serialize your model periodically as a checkpoint.
                //torch::save(net, "net.pt");
            }
        }
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        std::cout << "elapsed time: " << elapsed << " ms" << std::endl;
    }
    //send tensor through trained model:
    int n = 784;
    torch::Tensor alternatingTensor= torch::zeros({1,n});
    for (int i =0; i < n; i++){
        alternatingTensor[0][i] = i%2==0?1:0.5;
    }
    std::cout << "creating complete" << std::endl;
    std::cout << alternatingTensor << std::endl;
    alternatingTensor = alternatingTensor.to(device);
    std::cout << alternatingTensor << std::endl;
    std::cout << eval << std::endl;
    std::cout << "to device" << std::endl;
    torch::Tensor result = eval.forward(alternatingTensor);
    std::cout << "forward" << std::endl;
    std::cout << result << std::endl;
    std::cout << "serial: "<<eval.is_serializable() << std::endl;
    std::cout << "training: " <<eval.is_training() << std::endl;
    std::cout << "nodes: " << eval.layerNodes[0] << std::endl;
    std::cout << "nodes: " << eval.layerNodes[1] << std::endl;
    std::cout << "nodes: " << eval.layerNodes[2] << std::endl;
    std::cout << "nodes: " << eval.layerNodes[3] << std::endl;

    std::string filename = "evalModel.zip";
    eval.saveParameters(filename);


    std::string chessData = "C:/Universiteit/5-Artificiele_Intelligentie/Practicum4/UA_ChessAI_Project/ChessData";
    auto chessdataset = ChessDataset(chessData,34);
    std::cout << "dataset complete" << std::endl;
    ChessAIOptions options;
    ChessAITrainer trainer = ChessAITrainer(chessdataset,options);
    trainer.start();
    system("pause");
    return 0;
    auto chess_data_loader = torch::data::make_data_loader(
            std::move(chessdataset.map(torch::data::transforms::Stack<>())),
            torch::data::DataLoaderOptions().batch_size(128).workers(4).max_jobs(8).enforce_ordering(false));



    uint64_t counter = 0;
    start = std::chrono::steady_clock::now();
    for (auto& batch : *chess_data_loader) {
        //std::cout <<batch.data<< std::endl;
        //std::cout << "Batch size: " << batch.data.size(0) << " | Labels";
        // std::cout << batch.target;
        //std::cout << std::endl;
        counter += batch.data.size(0);
        if (counter%(128*200) == 0){
            elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count();
            if (elapsed == 0) continue;
            std::cout << counter << " elements" << "\t";
            std::cout << elapsed << " s" << "\t";
            std::cout << counter/elapsed << " elements/s" << std::endl;
        }
    }
    elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count();
    std::cout << counter << " elements" << std::endl;
    std::cout << elapsed << " s" << std::endl;
    std::cout << counter/elapsed << " elements/s" << std::endl;
    system("pause");
}