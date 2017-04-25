#!/bin/bash
echo -e "\n\n\nSolution: 0.0 "$(sort -n $1 | cut -d":" -f2 | tr "\n" " ") > $2
