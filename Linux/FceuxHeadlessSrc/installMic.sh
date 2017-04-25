#!/bin/bash
scons OPSEVENUP=0 MMIC=1
scons OPSEVENUP=0 MMIC=1 install --prefix=$HOME/Documentos/Linux/FceuxMic
cd ..
./compilarMic.sh
cd FceuxMic
rm -r share
zip -r ../../FceuxMic.zip * 
