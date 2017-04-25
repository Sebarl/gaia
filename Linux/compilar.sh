#!/bin/sh
icpc -O3 -c -fPIC AI.cpp -o AI.o
icpc -O3 -c -fPIC ANN/RNN.cpp -o RNN.o
icpc -O3 -c -fPIC ANN/Neuron.cpp -o Neuron.o
icpc -O3 -c -fPIC ANN/InputNeuron.cpp -o InputNeuron.o
icpc -O3 -c -fPIC ANN/BiasNeuron.cpp -o BiasNeuron.o
icpc -O3 -c -fPIC ANN/HiddenNeuron.cpp -o HiddenNeuron.o
icpc -O3 -c -fPIC ANN/OutputNeuron.cpp -o OutputNeuron.o
icpc -O3 -c -fPIC ANN/CheapSigmoid.cpp -o CheapSigmoid.o
icpc -O3 -c -fPIC ANN/Identity.cpp -o Identity.o
icpc -O3 -c -fPIC ANN/Sigmoid.cpp -o Sigmoid.o
icpc -O3 -c -fPIC ANN/Tanh.cpp -o Tanh.o
icpc -O3 -c -fPIC ANN/Gaussian.cpp -o Gaussian.o
icpc -O3 -c -fPIC ANN/ReLU.cpp -o ReLU.o
icpc -O3 -shared -o AI.so AI.o RNN.o Neuron.o InputNeuron.o BiasNeuron.o HiddenNeuron.o OutputNeuron.o CheapSigmoid.o Identity.o Sigmoid.o Tanh.o Gaussian.o ReLU.o
cp -f AI.so 'Fceux Headless/bin/AI.so'
icpc -c -O3 -fPIC Analyzer.cpp -o Analyzer.o
icpc -O3 -shared -o Analyzer.so Analyzer.o
cp -f Analyzer.so 'Fceux Headless/bin/Analyzer.so'
icpc -c -O3 -fPIC Dumper.cpp -o Dumper.o
icpc -O3 -shared -o Dumper.so Dumper.o
cp -f Dumper.so 'Fceux Headless/bin/Dumper.so'
icpc -c -O3 -fPIC DumpPlayer.cpp -o DumpPlayer.o
icpc -O3 -shared -o DumpPlayer.so DumpPlayer.o
cp -f DumpPlayer.so 'Fceux/bin/DumpPlayer.so'
