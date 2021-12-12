import torch
import torch.nn as nn

from dataloaders import Options


class EvalAgent(nn.Module):
    def __init__(self, options: Options):
        super().__init__()
        """ START TODO: fill in all three layers. 
            Remember that each layer should contain 2 parts, a linear layer and a nonlinear activation function.
            Use options.hidden_sizes to store all hidden sizes, (for simplicity, you might want to 
            include the input and output as well).
        """
        self.options=Options
        self.options.hidden_sizes = [784, 512, 32, 32,1]
        self.layer1 = nn.Sequential(
            torch.nn.Linear(self.options.hidden_sizes[0], self.options.hidden_sizes[1]),
            torch.nn.ReLU()
        )
        self.layer2 = nn.Sequential(
            torch.nn.Linear(self.options.hidden_sizes[1], self.options.hidden_sizes[2]),
            torch.nn.ReLU()
        )
        self.layer3 = nn.Sequential(
            torch.nn.Linear(self.options.hidden_sizes[2], self.options.hidden_sizes[3]),
            torch.nn.Softplus(beta=2,threshold=10)
        )
        self.layer4 = nn.Sequential(
            torch.nn.Linear(self.options.hidden_sizes[3], self.options.hidden_sizes[4]),
        )
        """END TODO"""

    def forward(self, x: torch.Tensor):
        """START TODO: forward tensor x through all layers."""
        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)
        """END TODO"""
        return x
