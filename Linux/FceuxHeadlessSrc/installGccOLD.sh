#!/bin/bash

scons install OPSEVENUP=0 --prefix=../Fceux\ Headless
cd ..
./compilarGcc.sh
