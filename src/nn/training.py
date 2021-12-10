import asyncio
import random
import time

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
    optimizer= torch.optim.SGD(agent.parameters(),lr=0.000001)
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
                print(f'epoch [{epoch + 1}/{options.num_epochs}] [{batchCount}/{len(dataloader)}]: ', end="")
                print(f"Running loss = {running_loss / (batchCount)}")


if __name__ == "__main__":
    options = Options()
    init_pytorch(options)
    # dataset = ChessDataset("C:/Universiteit/5-Artificiele_Intelligentie/Practicum4/UA_ChessAI_Project/ChessData",0,
    #                       options)
    batches = list(range(34,35))
    datasets = []
    for batch in batches:
        datasets.append(ChessDataset(
            "C:/Universiteit/5-Artificiele_Intelligentie/Practicum4/UA_ChessAI_Project"
                                     "/ChessData", batch, options))
    concatDataSets = torch.utils.data.ConcatDataset(datasets)
    data = torch.utils.data.DataLoader(datasets[0],batch_size = 128,shuffle=True, num_workers = 4,pin_memory=True,
                                       persistent_workers=True)
    f = open('C:/Universiteit/5-Artificiele_Intelligentie/Practicum4/UA_ChessAI_Project/ChessData/FEN/FenBatch00.txt','r')
    print(f.readline())



    brd = PyChess.Board()
    print(brd.pawns)
    print(batches)

    evalAgentTraining(options,data)
    print("hello world")
