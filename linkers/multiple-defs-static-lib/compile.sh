#!/bin/bash
set -xue

CC=clang++

${CC} -g -o a.o -fPIC -c a.cpp
ar rcs liba.a a.o

${CC} -g -o b.o -fPIC -c b.cpp
ar rcs libb.a b.o

${CC} -g -o c.o -fPIC -c c.cpp
ar rcs libc.a c.o

${CC} -g -o aa.o -fPIC -c aa.cpp
ar rcs libaa.a aa.o

${CC} -g -o libshared.so -shared shared.cpp -Wl,--whole-archive liba.a libb.a libc.a -Wl,--no-whole-archive # libaa.a
${CC} -g -o main main.cpp liba.a -rpath . libshared.so liba.a 
