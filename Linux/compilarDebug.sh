#!/bin/sh
icpc -g -c -fPIC AI.cpp -o AI.o
icpc -g -c -fPIC ANN/RNN.cpp -o RNN.o
icpc -g -c -fPIC ANN/Neuron.cpp -o Neuron.o
icpc -g -c -fPIC ANN/InputNeuron.cpp -o InputNeuron.o
icpc -g -c -fPIC ANN/BiasNeuron.cpp -o BiasNeuron.o
icpc -g -c -fPIC ANN/HiddenNeuron.cpp -o HiddenNeuron.o
icpc -g -c -fPIC ANN/OutputNeuron.cpp -o OutputNeuron.o
icpc -g -c -fPIC ANN/CheapSigmoid.cpp -o CheapSigmoid.o
icpc -g -c -fPIC ANN/Identity.cpp -o Identity.o
icpc -g -c -fPIC ANN/Sigmoid.cpp -o Sigmoid.o
icpc -g -shared -o AI.so AI.o RNN.o Neuron.o InputNeuron.o BiasNeuron.o HiddenNeuron.o OutputNeuron.o CheapSigmoid.o Identity.o Sigmoid.o
cp -f AI.so 'Fceux Headless/bin/AI.so'
icpc -c -g -fPIC Analyzer.cpp -o Analyzer.o
icpc -g -shared -o Analyzer.so Analyzer.o
cp -f Analyzer.so 'Fceux Headless/bin/Analyzer.so'
icpc -c -g -fPIC Dumper.cpp -o Dumper.o
icpc -g -shared -o Dumper.so Dumper.o
cp -f Dumper.so 'Fceux Headless/bin/Dumper.so'
icpc -c -g -fPIC DumpPlayer.cpp -o DumpPlayer.o
icpc -g -shared -o DumpPlayer.so DumpPlayer.o
cp -f DumpPlayer.so 'Fceux/bin/DumpPlayer.so'
