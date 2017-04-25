#!/bin/sh
g++ -O3 -c -fPIC AI.cpp -o AI.o
g++ -O3 -c -fPIC ANN/RNN.cpp -o RNN.o
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
g++ -O3 -shared -o AI.so AI.o RNN.o Neuron.o InputNeuron.o BiasNeuron.o HiddenNeuron.o OutputNeuron.o CheapSigmoid.o Identity.o Sigmoid.o Tanh.o Gaussian.o ReLU.o
cp -f AI.so 'Fceux Headless/bin/AI.so'
g++ -O3 -c -fPIC Analyzer.cpp -o Analyzer.o
g++ -O3 -shared -o Analyzer.so Analyzer.o
cp -f Analyzer.so 'Fceux Headless/bin/Analyzer.so'
g++ -c -O3 -fPIC Dumper.cpp -o Dumper.o
g++ -O3 -shared -o Dumper.so Dumper.o
cp -f Dumper.so 'Fceux Headless/bin/Dumper.so'
g++ -c -O3 -fPIC DumpPlayer.cpp -o DumpPlayer.o
g++ -O3 -shared -o DumpPlayer.so DumpPlayer.o
cp -f DumpPlayer.so 'Fceux/bin/DumpPlayer.so'
