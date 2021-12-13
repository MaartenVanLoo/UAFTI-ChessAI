#include <torch/torch.h>
#include "evalModel.h"


Eval::Eval(){
    //Constrct and register layers
    fc1 = register_module("fc1",torch::nn::Linear(layerNodes[0],layerNodes[1]));
    fc2 = register_module("fc2",torch::nn::Linear(layerNodes[1],layerNodes[2]));
    fc3 = register_module("fc3",torch::nn::Linear(layerNodes[2],layerNodes[3]));
}

// Implement the Net's algorithm.
torch::Tensor Eval::forward(torch::Tensor x) {
    // Use one of many tensor manipulation functions.
    x = torch::relu(fc1->forward(x.reshape({x.size(0), layerNodes[0]})));
    x = torch::relu(fc2->forward(x));
    x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
    return x;
}
void Eval::saveParameters(std::string& filename){
    std::vector<torch::Tensor> vec = {
            fc1->weight, fc1->bias,
            fc2->weight, fc2->bias,
            fc3->weight, fc3->bias
    };
    std::vector<char> chars = torch::pickle_save(vec);
    std::ofstream ofstream(filename, std::ios::out | std::ios::binary);
    ofstream.write(chars.data(), chars.size());
    ofstream.close();
}


