#include <torch/torch.h>
#include <iostream>
#include "evalModel.h"
#include "ChessDataset.h"

int main() {
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
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
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
    auto chess_data_loader = torch::data::make_data_loader(
            std::move(chessdataset),
            torch::data::DataLoaderOptions().batch_size(128).workers(4).max_jobs(2));
    for (torch::data::Example<>& batch : *data_loader) {
        std::cout << "Batch size: " << batch.data.size(0) << " | Labels: ";
        for (int64_t i = 0; i < batch.data.size(0); ++i) {
            std::cout << batch.target[i].item<int64_t>() << " ";
        }
        std::cout << std::endl;
        break;
    }
    system("pause");
}