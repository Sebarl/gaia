#!/bin/bash

./Evaluator $1/Battle_City/RAMMask.mask $1/Battle_City/analyzer.weights ../TestMovies/Battle_City/ $1/Battle_City/framesToDrop.drop Battle_City.nes > $1/Battle_City/out

./Evaluator $1/Pacman/RAMMask.mask $1/Pacman/analyzer.weights ../TestMovies/Pacman/ $1/Pacman/framesToDrop.drop Pacman.nes > $1/Pacman/out

./Evaluator $1/Pinball/RAMMask.mask $1/Pinball/analyzer.weights ../TestMovies/Pinball/ $1/Pinball/framesToDrop.drop Pinball.nes > $1/Pinball/out

./Evaluator $1/SuperMarioBros/RAMMask.mask $1/SuperMarioBros/analyzer.weights ../TestMovies/SuperMarioBros/ $1/SuperMarioBros/framesToDrop.drop SuperMarioBros.nes > $1/SuperMarioBros/out

./Evaluator $1/Tetris/RAMMask.mask $1/Tetris/analyzer.weights ../TestMovies/Tetris/ $1/Tetris/framesToDrop.drop Tetris.nes > $1/Tetris/out

./Evaluator $1/Ice_Hockey/RAMMask.mask $1/Ice_Hockey/analyzer.weights ../TestMovies/Ice_Hockey/ $1/Ice_Hockey/framesToDrop.drop Ice_Hockey.nes > $1/Ice_Hockey/out

./Evaluator $1/Space_Invaders/RAMMask.mask $1/Space_Invaders/analyzer.weights ../TestMovies/Space_Invaders/ $1/Space_Invaders/framesToDrop.drop Space_Invaders.nes > $1/Space_Invaders/out

./Evaluator $1/Contra/RAMMask.mask $1/Contra/analyzer.weights ../TestMovies/Contra/ $1/Contra/framesToDrop.drop Contra.nes > $1/Contra/out
