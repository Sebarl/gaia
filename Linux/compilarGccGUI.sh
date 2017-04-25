#!/bin/sh
g++ -O3 -c -fPIC AIGUI.cpp -o AIGUI.o
g++ -O3 -c -fPIC ANN/RNNGUI.cpp -o RNNGUI.o
g++ -O3 -c -fPIC ANN/Neuron.cpp -o Neuron.o
g++ -O3 -c -fPIC ANN/InputNeuron.cpp -o InputNeuron.o
g++ -O3 -c -fPIC ANN/BiasNeuron.cpp -o BiasNeuron.o
g++ -O3 -c -fPIC ANN/HiddenNeuron.cpp -o HiddenNeuron.o
g++ -O3 -c -fPIC ANN/OutputNeuron.cpp -o OutputNeuron.o
g++ -O3 -c -fPIC ANN/CheapSigmoid.cpp -o CheapSigmoid.o
g++ -O3 -c -fPIC ANN/Identity.cpp -o Identity.o
g++ -O3 -c -fPIC ANN/Sigmoid.cpp -o Sigmoid.o
g++ -O3 -c -fPIC ANN/Tanh.cpp -o Tanh.o
g++ -O3 -c -fPIC ANN/Gaussian.cpp -o Gaussian.o
g++ -O3 -c -fPIC ANN/ReLU.cpp -o ReLU.o
g++ -O3 -shared -o AIGUI.so AIGUI.o RNNGUI.o Neuron.o InputNeuron.o BiasNeuron.o HiddenNeuron.o OutputNeuron.o CheapSigmoid.o Identity.o Sigmoid.o Tanh.o Gaussian.o ReLU.o
cp -f AIGUI.so Fceux/bin/AI.so
