#!/bin/bash
cd VideoEvaluator
g++ -O3 -fopenmp -lgomp -o Evaluator main.cpp GenericException.cpp
