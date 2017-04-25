#!/bin/bash

pushd $1

mkdir -p outs/Good
mkdir -p outs/Bad
mkdir -p outs/Avg

gawk 'BEGIN {startGoodVideo = "^Video .*/Good/(.*).fm2$"; startBadVideo = "^Video .*/Bad/(.*).fm2$"; endVideo = "^El video .*tuvo (.*) fitness$"; startAvgGood="^Valor de .*buenos:$"; startAvgBad="^Valor de .*malos:$"} {} $0 ~ startGoodVideo {file_name= "outs/Good/"gensub(startGoodVideo, "\\1", "g")".out";print "Reading "file_name;out="";next;} $0 ~ startBadVideo {file_name= "outs/Bad/"gensub(startBadVideo, "\\1", "g")".out";print "Reading "file_name;out="";next;} $0 ~ endVideo {print "\tFitness: "gensub(endVideo, "\\1", "g");print out > file_name;next;} $0 ~ startAvgGood {print out > file_name; file_name="outs/Avg/avgGood.out"; print "Reading avgGood"; out ="";next;} $0 ~ startAvgBad {print out > file_name; file_name="outs/Avg/avgBad.out"; print "Reading avgBad"; out ="";next;} (NF == 0){next;} {out=out $0 "\n" } END {print out > file_name}' out

popd
