#!/bin/bash

g++ -O3 -o Diagnostics/RNNStats/RNNStats Diagnostics/RNNStats/main.cpp ANN/OutputNeuron.cpp ANN/BiasNeuron.cpp ANN/CheapSigmoid.cpp ANN/HiddenNeuron.cpp ANN/Identity.cpp ANN/InputNeuron.cpp ANN/RNN.cpp ANN/Neuron.cpp ANN/Sigmoid.cpp ANN/Tanh.cpp ANN/Gaussian.cpp ANN/ReLU.cpp

