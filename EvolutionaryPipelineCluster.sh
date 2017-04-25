#!/bin/bash

line=""
play=false
while getopts ":p" opt; do
  case $opt in
    p)
      play=true
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

cd Analyzer

echo "Locating training videos..."
cantGood=0
for file in ../TestMovies/$(echo $1| cut -d"." -f1)/Good/*
do
line=$line" ../"$file
cantGood=$(($cantGood + 1))
done

for file in ../TestMovies/$(echo $1| cut -d"." -f1)/Bad/*
do
line=$line" ../"$file
done

echo "Training videos located successfully"

echo "Starting RAM analysis..."
./AnalyzeNodo40 $1 $line $cantGood > RAMMask.mask
echo "RAM analysis complete"

RAMCOUNT=$(wc -l stage2.weights | cut -d" " -f1)

cd ..

echo "Starting objective function optimization..."
./ObjectiveInfererNodo40 paramsObjective.txt 1 Analyzer/stage2.weights TestMovies/$(echo $1| cut -d"." -f1) $RAMCOUNT $1 > Objectives.weights
echo "Objective function optimization complete"

echo "Starting ANN training..."
DATE=$(date +"%Y-%m-%d_%H-%M-%S")
./Nodo40NEAT paramsNEAT.txt 1 $RAMCOUNT ../../Analyzer/RAMMask.mask Analyzer/prefix ../Analyzer/framesToDrop.drop ../../Objectives.weights $1 > Solutions/$(echo $1| cut -d"." -f1)/player$DATE.solution
echo "ANN training complete"

echo "AI generation complete"
