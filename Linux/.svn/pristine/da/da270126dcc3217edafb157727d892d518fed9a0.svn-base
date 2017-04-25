#!/bin/sh
icpc -c -O3 -fPIC AI.cpp -o AI.o
icpc -O3 -shared -o AI.so AI.o
cp -f AI.so 'Fceux Headless/bin/AI.so'
icpc -c -O3 -fPIC Analyzer.cpp -o Analyzer.o
icpc -O3 -shared -o Analyzer.so Analyzer.o
cp -f Analyzer.so 'Fceux Headless/bin/Analyzer.so'
icpc -c -O3 -fPIC Dumper.cpp -o Dumper.o
icpc -O3 -shared -o Dumper.so Dumper.o
cp -f Dumper.so 'Fceux Headless/bin/Dumper.so'
icpc -c -O3 -fPIC DumpPlayer.cpp -o DumpPlayer.o
icpc -O3 -shared -o DumpPlayer.so DumpPlayer.o
cp -f DumpPlayer.so 'Fceux/bin/DumpPlayer.so'
