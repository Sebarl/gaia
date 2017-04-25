#!/bin/sh
g++ -g -c -fPIC AIGUI.cpp -o AIGUI.o
g++ -g -c -fPIC ANN/RNNGUI.cpp -o RNNGUI.o
g++ -g -c -fPIC ANN/Neuron.cpp -o Neuron.o
g++ -g -c -fPIC ANN/InputNeuron.cpp -o InputNeuron.o
g++ -g -c -fPIC ANN/BiasNeuron.cpp -o BiasNeuron.o
g++ -g -c -fPIC ANN/HiddenNeuron.cpp -o HiddenNeuron.o
g++ -g -c -fPIC ANN/OutputNeuron.cpp -o OutputNeuron.o
g++ -g -c -fPIC ANN/CheapSigmoid.cpp -o CheapSigmoid.o
g++ -g -c -fPIC ANN/Identity.cpp -o Identity.o
g++ -g -c -fPIC ANN/Sigmoid.cpp -o Sigmoid.o
g++ -g -shared -o AIGUI.so AIGUI.o RNNGUI.o Neuron.o InputNeuron.o BiasNeuron.o HiddenNeuron.o OutputNeuron.o CheapSigmoid.o Identity.o Sigmoid.o
cp -f AIGUI.so Fceux/bin/AI.so
