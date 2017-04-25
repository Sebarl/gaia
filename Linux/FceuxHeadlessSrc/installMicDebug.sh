#!/bin/bash
scons OPSEVENUP=0 RELEASE=0 MMIC=1 DEBUG=1
scons OPSEVENUP=0 RELEASE=0 MMIC=1 DEBUG=1 install --prefix=$HOME/Documentos/Linux/FceuxMic
cd ..
./compilarMicDebug.sh
cd FceuxMic
rm -r share
zip -r ../../FceuxMicDebug.zip * 
