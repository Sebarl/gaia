#!/bin/bash

function run(){
		echo "114 162 14 66 1 123 182 14 64 0 129 163 8 64 0 153 193 14 2 0 170 166 15 64 0 145 200 11 66 0 116 181 12 66 1 113 156 0 66 0 121 157 10 66 1 161 183 19 2 1 132 192 3 66 1 145 171 2 64 0 170 187 14 66 0 86 184 0 64 1 190 205 0 2 0 149 217 0 2 1 141 204 6 66 0 164 208 0 66 1 163 195 0 66 0 105 170 0 2 1 $(($1+$2)) 5 94 23" > ae$2.txt &
		result=$(./fceux -e ae$2.txt -l 1000000 Pinball.nes)
		blah=$(echo $result | cut -d" " -f1)
		echo $blah";"$(($1+$2)) >> resultados.txt

}

for i in {0..79..8}
do
	for j in {0..7}
	do
		run $i $j &
	done
	wait
done
