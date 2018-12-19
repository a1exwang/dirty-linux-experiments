#!/bin/bash
cat ub1.cpp
clang++ -O2 -S -o - -c ub1.cpp
clang++ -O2 -o main infinite-loop-without-side-effects.cpp
./main
rm main
