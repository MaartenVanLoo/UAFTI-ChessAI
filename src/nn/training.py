import asyncio
import random
import time
from datetime import datetime

import torch
import torch.utils
import torch.utils.data
import PyChess
import dataloaders
from dataloaders import ChessDataset
from dataloaders import Options
from model import EvalAgent


def init_pytorch(options: Options):
    # set all random seeds for reproducibility
    torch.manual_seed(options.random_seed)
    torch.cuda.manual_seed(options.random_seed)
    random.seed(options.random_seed)
    # set device
    if options.device[:4] == "cuda" and torch.cuda.is_available():
        print("Training on GPU")
        options.device = torch.device("cuda:0")
    else:
        options.device = torch.device("cpu")

def evalAgentTraining(options:Options,dataloader:torch.utils.data.DataLoader):
    agent = EvalAgent(options).to(options.device)
    criterion = torch.nn.MSELoss()
    optimizer= torch.optim.SGD(agent.parameters(),lr=0.000002)
    start_time = time.time()
    for epoch in range(0,options.num_epochs):
        running_loss = 0
        batchCount = 0
        for (x,y) in dataloader:
            optimizer.zero_grad()
            x = x.to(options.device)
            y = y.to(options.device)
            x = torch.reshape(x, (x.shape[0], -1))
            x = agent(x)
            loss = criterion(x, y)
            loss.backward()
            optimizer.step()
            running_loss += loss.item()
            batchCount+=1
            if batchCount%200==0:
                now=datetime.now()
                string_time = ('%02d:%02d:%02d.%d'%(now.hour,now.minute,now.second,now.microsecond))[:-3]
                print(f'[{string_time}]:epoch [{epoch + 1}/{options.num_epochs}] [{batchCount}/{len(dataloader)}]: ', end="")
                print(f"Running loss = {running_loss / (batchCount)}")
        torch.save(agent.state_dict(), f"State{epoch+1}.model")
        torch.save(agent, f"State{epoch+1}.pt")

def toONNX(path):
    #load model parameters
    params = torch.load(path)
    print(params)

    #make agent
    options = Options();
    agent = EvalAgent(options);

    #set weights
    agent.layer1[0].weight = torch.nn.Parameter(params[0])
    agent.layer1[0].bias = torch.nn.Parameter(params[1])
    agent.layer2[0].weight = torch.nn.Parameter(params[2])
    agent.layer2[0].bias = torch.nn.Parameter(params[3])
    agent.layer3[0].weight = torch.nn.Parameter(params[4])
    agent.layer3[0].bias = torch.nn.Parameter(params[5])
    #agent.layer4[0].weight = torch.nn.Parameter(params[6])
    #agent.layer4[0].bias = torch.nn.Parameter(params[7])

    #check result of pseudo random tensor:
    randomTensor = torch.load("random.tensor")
    print(randomTensor)
    device= torch.device("cpu")
    randomTensor[0] = randomTensor[0].to(device)
    agent = agent.to(device)
    result = agent(randomTensor[0])
    print(result)
    print(randomTensor[1])
    if (torch.eq(result,randomTensor[1]).data):
        print("Result correct")
    else:
        print("Something went wrong?")

    # set the model to inference mode
    agent.eval()

    # input to the model
    x = torch.randn(1,784,requires_grad  = True)
    torch_out = agent(x)

    #export the model
    torch.onnx.export(agent,
                      x,
                      "ChessAI.onnx",
                      training=torch.onnx.TrainingMode.EVAL,
                      export_params = True,
                      opset_version = 13,
                      input_names = ['input'],
                      output_names = ['output'],
                      dynamic_axes={'input': {0: 'batch_size'},  # variable length axes
                                    'output': {0: 'batch_size'}}
    )



if __name__ == "__main__":
    path = "model01.param"
    toONNX(path)
    quit();

    now=datetime.now()
    print(('%02d:%02d:%02d.%06d'%(now.hour,now.minute,now.second,now.microsecond))[:-3])
    options = Options()
    init_pytorch(options)
    # dataset = ChessDataset("C:/Universiteit/5-Artificiele_Intelligentie/Practicum4/UA_ChessAI_Project/ChessData",0,
    #                       options)
    batches = list(range(34,35))
    datasets = []
    for batch in batches:
        datasets.append(ChessDataset(
            "C:/Universiteit/5-Artificiele_Intelligentie/Practicum4/UA_ChessAI_Project"
            #"D:/Documenten/Universiteit/5-AI"
                                     "/ChessData", batch, options))
    concatDataSets = torch.utils.data.ConcatDataset(datasets)
    data = torch.utils.data.DataLoader(datasets[0],batch_size = 128,shuffle=True, num_workers = 4,pin_memory=True,
                                       persistent_workers=True)
    #f = open('C:/Universiteit/5-Artificiele_Intelligentie/Practicum4/UA_ChessAI_Project/ChessData/FEN/FenBatch00.txt','r')
    #print(f.readline())



    brd = PyChess.Board()
    print(brd.pawns)
    print(batches)

    evalAgentTraining(options,data)
    print("hello world")
