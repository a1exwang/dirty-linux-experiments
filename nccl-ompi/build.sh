#!/bin/bash
set -xe

mkdir -p build
cd build
nvcc $INCLUDE -g -std=c++11 -x cu -dc ../allreduce.cu -o allreduce.cu.o
g++ $INCLUDE -g -std=c++11 -o main.cpp.o -c ../main.cpp
nvcc -g -Xcompiler=-fPIC -Wno-deprecated-gpu-targets -shared -dlink allreduce.cu.o ../main.cpp.o -o nv_dlink.o -lnccl -lcublas -Xnvlink -lmpi_cxx -Xnvlink -lmpi -lcudadevrt -lcudart_static -lrt -lpthread -ldl
g++ -g main.cpp.o nv_dlink.o allreduce.cu.o -o nccl-ompi-allreduce -lnccl -lcublas -lmpi_cxx -lmpi -lcudadevrt -lcudart_static -lrt -lpthread -ldl
