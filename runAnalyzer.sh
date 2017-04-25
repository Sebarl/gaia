#!/bin/bash

cd Linux/Analyzer

#Games=("Contra.nes" "Pinball.nes" "SuperMarioBros.nes" "Battle_City.nes" "Ice_Hockey.nes" "Space_Invaders.nes" "Tetris.nes" "Pacman.nes")

Games=("Pinball.nes" "Tetris.nes")

for i in ${Games[@]}; do
    echo "Performing analysis for $i"
    echo "Locating training videos..."
    cantGood=0
    line=""
    for file in ../TestMovies/$(echo $i| cut -d"." -f1)/Good/*
    do
    line=$line" ../"$file
    cantGood=$(($cantGood + 1))
    done

    for file in ../TestMovies/$(echo $i| cut -d"." -f1)/Bad/*
    do
    line=$line" ../"$file
    done

    echo "Training videos located successfully"

    echo "Starting RAM analysis..."
    FSTSTARTTIME=$(date +%s)
    #echo $i $line $cantGood
    ./bin/Release/Analyzer  $i $line $cantGood > RAMMask.mask
    ENDTIME=$(date +%s)
    echo "RAM analysis complete ("$(($ENDTIME - $FSTSTARTTIME))" seconds)"
    cd Resultados    
    mkdir -p $(echo $i| cut -d"." -f1)
    cd $(echo $i| cut -d"." -f1)
    mv ../../prefix .
    mv ../../stage2.weights .
    mv ../../stage2.mask .
    mv ../../framesToDrop.drop .
    mv ../../RAMMask.mask .    
    cd ../../
done
