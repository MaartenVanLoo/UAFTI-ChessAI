//
// Created by maart on 9/12/2021.
//

#ifndef UAFTI_CHESSAI_EVALMODEL_H
#define UAFTI_CHESSAI_EVALMODEL_H


struct Eval : torch::nn::Module{
    const int layerNodes[4] = {784,784,784,10};
    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
    Eval();
    torch::Tensor forward(torch::Tensor x);
};


#endif //UAFTI_CHESSAI_EVALMODEL_H
