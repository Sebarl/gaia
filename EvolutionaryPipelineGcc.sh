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

cd Linux/Analyzer

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
FSTSTARTTIME=$(date +%s)
./bin/Release/Analyzer $1 $line $cantGood > RAMMask.mask
ENDTIME=$(date +%s)
echo "RAM analysis complete ("$(($ENDTIME - $FSTSTARTTIME))" seconds)"

RAMCOUNT=$(wc -l stage2.weights | cut -d" " -f1)

cd ../../ObjectInfererAE

echo "Starting objective function optimization..."
STARTTIME=$(date +%s)
./bin/Release/OwnPinballAE params.txt 1 ../Linux/Analyzer/stage2.weights ../Linux/Analyzer/framesToDrop.drop ../Linux/TestMovies/$(echo $1| cut -d"." -f1) $RAMCOUNT $1 > Objectives.weights
ENDTIME=$(date +%s)
echo "Objective function optimization complete ("$(($ENDTIME - $STARTTIME))" seconds)"

cd ../ANNTrainerAE/

echo "Starting ANN training..."
DATE=$(date +"%Y-%m-%d_%H-%M-%S")
STARTTIME=$(date +%s)
./bin/Release/OwnPinballAE params.txt 1 $RAMCOUNT ../../Analyzer/RAMMask.mask ../Linux/Analyzer/prefix ../../../ObjectInfererAE/Objectives.weights $1 > ../Linux/Solutions/$(echo $1| cut -d"." -f1)/player$DATE.solution
ENDTIME=$(date +%s)
echo "ANN training complete ("$(($ENDTIME - $STARTTIME))" seconds)"

echo "AI generation complete ("$(($ENDTIME - $FSTSTARTTIME))" seconds)"

if [ "$play" == true ]; then
	cd ../Linux/Fceux/bin
	echo OHMYGAT
	./fceux -e ../../Solutions/$(echo $1| cut -d"." -f1)/player$DATE.solution $1 &
fi
