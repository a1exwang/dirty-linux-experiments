FROM registry.cn-huhehaote.aliyuncs.com/momenta-aiplatform/rocs:v0.5.1

ADD nccl_2.2.13-1+cuda9.0_x86_64.txz /usr/local
RUN yum install -y epel-release
RUN yum install -y cmake3 openmpi-devel
RUN yum install -y make gcc gcc-c++

RUN mkdir /wtf
ADD mmpl-1.2.1 /wtf/mmpl-1.2.1
ADD mmpl-1.2.2 /wtf/mmpl-1.2.2
ADD CMakeLists.txt *.cpp *.cu *.h /wtf/
RUN cd /wtf && mkdir build && cd build && cmake3 .. && make

#CMD mpirun --allow-run-as-root -np 1 /wtf/build/nccl-ompi-allreduce 65536
CMD mpirun --allow-run-as-root -np 1 /wtf/build/mmpl_main

RUN git clone https://github.com/NVIDIA/nccl-tests.git /nccl-tests
RUN cd /nccl-tests && \
  make MPI=1 MPI_HOME=/usr/local/openmpi-3.0.0 NCCL_HOME=/usr/local/nccl_2.2.13-1+cuda9.0_x86_64/

ADD bandwidthTest /bandwidthTest
RUN ln -sf /usr/lib64/libcuda.so.410.66 /usr/lib64/libcuda.so.1 && \
  ln -sf /usr/lib64/libnvidia-ml.so.410.66 /usr/lib64/libnvidia-ml.so.1

