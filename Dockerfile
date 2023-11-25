FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt install -y --no-install-recommends \
    build-essential cmake vim openssl curl ca-certificates \
    ninja-build wget lsb-release software-properties-common \
    gnupg zlib1g-dev libcurl4-openssl-dev libedit-dev libzstd-dev

# Set the working directory to root (ie $HOME)
WORKDIR root

RUN curl -O https://apt.llvm.org/llvm.sh

RUN chmod u+x llvm.sh

RUN ./llvm.sh 17 -y

COPY . .

RUN ln -s /usr/bin/clang-17 /usr/bin/clang     && \
    ln -s /usr/bin/clang++-17 /usr/bin/clang++ && \
    ln -s /usr/bin/opt-17 /usr/bin/opt         && \
    ln -s /usr/bin/llvm-symbolizer-17 /usr/bin/llvm-symbolizer

RUN cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=clang-17 -DCMAKE_CXX_COMPILER=clang++-17 \
    -DBuildVisualTools=FALSE

RUN ninja -C build