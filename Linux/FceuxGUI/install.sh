#!/bin/bash

scons install --prefix=../Fceux
cd ..
./compilarGccGUI.sh
