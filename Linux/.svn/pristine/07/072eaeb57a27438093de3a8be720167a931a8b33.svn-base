#!/bin/sh
g++ -O3 -c -fPIC AI.cpp -o AI.o
g++ -O3 -shared -o AI.so AI.o
cp -f AI.so 'Fceux Headless/bin/AI.so'
g++ -O3 -c -fPIC Analyzer.cpp -o Analyzer.o
g++ -O3 -shared -o Analyzer.so Analyzer.o
cp -f Analyzer.so 'Fceux Headless/bin/Analyzer.so'
g++ -c -O3 -fPIC Dumper.cpp -o Dumper.o
g++ -O3 -shared -o Dumper.so Dumper.o
cp -f Dumper.so 'Fceux Headless/bin/Dumper.so'
g++ -c -O3 -fPIC DumpPlayer.cpp -o DumpPlayer.o
g++ -O3 -shared -o DumpPlayer.so DumpPlayer.o
cp -f DumpPlayer.so 'Fceux/bin/DumpPlayer.so'
