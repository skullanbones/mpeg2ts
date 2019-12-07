#
# Simple Dockerfile for building and debugging ts-lib with necessary tools.
#

FROM ubuntu:18.04 as base


RUN apt-get update && apt-get --yes --no-install-recommends install \
    ca-certificates \
    curl \
    gnupg2 \
    gosu \
    apt-utils \
    software-properties-common \
    build-essential \
    tstools \
    gdb \
    gdbserver \
    wget \
    lcov \
    sudo \
    zip \
    unzip \
    doxygen \
    graphviz && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

### Builder image ###
FROM base as build

# LLVM Clang
ENV CLANG_VERSION=7
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-$CLANG_VERSION main" && \
apt-get update && \
apt-get install --yes --no-install-recommends clang-$CLANG_VERSION && \
apt-get install --yes --no-install-recommends clang-tidy-$CLANG_VERSION && \
apt-get install --yes --no-install-recommends clang-format-$CLANG_VERSION && \
apt-get install --yes --no-install-recommends llvm-$CLANG_VERSION-dev && \
apt-get install --yes --no-install-recommends libclang-$CLANG_VERSION-dev


# python support
RUN apt-get install --yes --no-install-recommends \
  virtualenv \
  python3-setuptools \
  python3-pip \
  python3-dev \
  python3-virtualenv \
  python3-pip && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*


#python packages
RUN pip3 install \
  flake8 \
  autopep8 \
  virtualenv

# Install latest git
RUN apt-add-repository ppa:git-core/ppa \
  && apt-get update \
  && apt-get install --yes --no-install-recommends git \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

# Install latest cmake
ENV CMAKE_VERSION=3.16
ENV CMAKE_BUILD_VERSION=0
RUN mkdir /opt/cmake && cd /opt/cmake && \
wget https://cmake.org/files/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.$CMAKE_BUILD_VERSION.tar.gz && \
tar -xzvf cmake-$CMAKE_VERSION.$CMAKE_BUILD_VERSION.tar.gz && \
cd cmake-$CMAKE_VERSION.$CMAKE_BUILD_VERSION/ && \
./bootstrap -- -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_USE_OPENSSL=OFF && \
make -j$(nproc) && \
make install && \
cmake --version && \
rm -rf /opt/cmake

# Install latest cppcheck
RUN git clone https://github.com/danmar/cppcheck.git /cppcheck \
  && mkdir /cppcheck/build \
  && cd /cppcheck/build \
  && cmake .. && cmake --build . -- -j16 && make install \
  && cd / && rm -rf /cppcheck

# Install gtest/gmock
RUN git clone -q https://github.com/google/googletest.git /googletest \
  && cd googletest \
  && git checkout tags/release-1.8.1 \
  && mkdir -p /googletest/build \
  && cd /googletest/build \
  && cmake .. && make && make install \
  && cd / && rm -rf /googletest

# Install benchmark
RUN git clone -q https://github.com/google/benchmark.git /benchmark \
  && cd benchmark \
  && git checkout tags/v1.4.0 \
  && mkdir -p /benchmark/build \
  && cd /benchmark/build \
  && cmake -DCMAKE_BUILD_TYPE=Release .. && make && make install \
  && cd / && rm -rf /benchmark

# Install latest iwyu
RUN git clone https://github.com/include-what-you-use/include-what-you-use.git /iwyu \
  && mkdir /iwyu/build \
  && cd /iwyu && git checkout clang_$CLANG_VERSION.0 \
  && cd /iwyu/build \
  && cmake .. && cmake --build . -- -j16 && make install \
  && cd / && rm -rf /iwyu

# Install as docker user (after entrypoint)
# Install Emscripten SDK/toolchain
ENV EMSDK_VERSION=1.38
ENV EMSDK_BUILD_VERSION=31
RUN git clone https://github.com/emscripten-core/emsdk.git /tmp/emsdk && \
cd /tmp/emsdk && \
./emsdk install sdk-tag-$EMSDK_VERSION.$EMSDK_BUILD_VERSION-64bit && \
./emsdk activate sdk-tag-$EMSDK_VERSION.$EMSDK_BUILD_VERSION-64bit
ENV PATH="${PATH}:/tmp/emsdk/emscripten/tag-$EMSDK_VERSION.$EMSDK_BUILD_VERSION"
RUN echo "PATH: " $PATH
RUN em++ --version

# entrypoint
COPY ./entrypoint.sh /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

WORKDIR /tmp/workspace
