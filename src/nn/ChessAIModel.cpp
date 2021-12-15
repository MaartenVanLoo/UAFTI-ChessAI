//
// Created by maart on 13/12/2021.
//

#include "ChessAIModel.h"

ChessAIModel::ChessAIModel() {
    this->f1 = torch::nn::Sequential(
        torch::nn::Linear(layerNodes[0],layerNodes[1]),
        torch::nn::ReLU()
    );
    this->f2 = torch::nn::Sequential(
        torch::nn::Linear(layerNodes[1],layerNodes[2]),
        torch::nn::ReLU()
    );
    this->f3 = torch::nn::Sequential(
        torch::nn::Linear(layerNodes[2],layerNodes[3])
        //torch::nn::ReLU()
    );/*
    this->f4 = torch::nn::Sequential(
        torch::nn::Linear(layerNodes[3],layerNodes[4])
    );*/
    register_module("f1",f1);
    register_module("f2",f2);
    register_module("f3",f3);
    //register_module("f4",f4);


}

torch::Tensor ChessAIModel::forward(torch::Tensor x) {
    x = f1->forward(x);
    x = f2->forward(x);
    x = f3->forward(x);
    //x = f4->forward(x);
    return x;
}

void ChessAIModel::saveParameters(std::string &filename) {
    std::vector<torch::Tensor> vec = {

    };
    for (auto& p : this->named_parameters()){
        std::cout << "name: " << std::setw(10) << std::left << p.key();
        std::cout << "\tshape: " << p.value().sizes() << std::endl;
        vec.push_back(p.value());
    }
    std::vector<char> chars = torch::pickle_save(vec);
    std::ofstream ofstream(filename, std::ios::out | std::ios::binary);
    ofstream.write(chars.data(), chars.size());
    ofstream.close();
}
