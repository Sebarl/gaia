#!/bin/bash

./convertWeights.sh $1/Battle_City/stage2.weights $1/Battle_City/analyzer.weights

./convertWeights.sh $1/Pacman/stage2.weights $1/Pacman/analyzer.weights

./convertWeights.sh $1/Pinball/stage2.weights $1/Pinball/analyzer.weights

./convertWeights.sh $1/SuperMarioBros/stage2.weights $1/SuperMarioBros/analyzer.weights

./convertWeights.sh $1/Tetris/stage2.weights $1/Tetris/analyzer.weights

./convertWeights.sh $1/Ice_Hockey/stage2.weights $1/Ice_Hockey/analyzer.weights

./convertWeights.sh $1/Space_Invaders/stage2.weights $1/Space_Invaders/analyzer.weights

./convertWeights.sh $1/Contra/stage2.weights $1/Contra/analyzer.weights
