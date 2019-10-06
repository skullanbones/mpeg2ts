#
# Simple Dockerfile for building and debugging ts-lib with necessary tools.
#
# Copyright © 2018 All rights reserved for skullanbones™
#

FROM ubuntu:18.04 as base

### Create user account ###
FROM base as user
RUN apt-get update && apt-get -y --no-install-recommends install \
    ca-certificates \
    curl \
    gnupg2 \
    gosu

### Builder image ###
FROM user as build

RUN apt-get update && apt-get install --yes --no-install-recommends
RUN apt-get install --yes --no-install-recommends apt-utils
RUN apt-get install --yes --no-install-recommends software-properties-common
RUN apt-get install --yes --no-install-recommends build-essential
RUN apt-get install --yes --no-install-recommends tstools
RUN apt-get install --yes --no-install-recommends gdb
RUN apt-get install --yes --no-install-recommends gdbserver
RUN apt-get install --yes --no-install-recommends wget
RUN apt-get install --yes --no-install-recommends lcov
RUN apt-get install --yes --no-install-recommends sudo
RUN apt-get install --yes --no-install-recommends zip
RUN apt-get install --yes --no-install-recommends unzip
RUN apt-get install --yes --no-install-recommends doxygen
RUN apt-get install --yes --no-install-recommends graphviz

# LLVM Clang
ENV CLANG_VERSION=7
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-$CLANG_VERSION main"
RUN apt-get update
RUN apt-get install --yes --no-install-recommends clang-$CLANG_VERSION
RUN apt-get install --yes --no-install-recommends clang-tidy-$CLANG_VERSION
RUN apt-get install --yes --no-install-recommends clang-format-$CLANG_VERSION
RUN apt-get install --yes --no-install-recommends llvm-$CLANG_VERSION-dev
RUN apt-get install --yes --no-install-recommends libclang-$CLANG_VERSION-dev


# python support
RUN apt-get install --yes --no-install-recommends python3-setuptools
RUN apt-get install --yes --no-install-recommends virtualenv
RUN apt-get install --yes --no-install-recommends python3-pip
RUN apt-get install --yes --no-install-recommends python3-dev
RUN apt-get install --yes --no-install-recommends python3-virtualenv
RUN apt-get install --yes --no-install-recommends python3-pip


#python packages
RUN pip3 install flake8
RUN pip3 install autopep8

# Install latest git
RUN apt-add-repository ppa:git-core/ppa \
  && apt-get update \
  && apt-get install --yes --no-install-recommends git \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

# Install latest cmake
RUN curl -o /cmake-3.13.1-Linux-x86_64.sh -SL "https://cmake.org/files/v3.13/cmake-3.13.1-Linux-x86_64.sh"
RUN mkdir /opt/cmake
RUN sh /cmake-3.13.1-Linux-x86_64.sh --prefix=/opt/cmake --skip-license
RUN ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
RUN cmake --version

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
RUN git clone https://github.com/emscripten-core/emsdk.git /tmp/emsdk
RUN cd /tmp/emsdk && ./emsdk install sdk-tag-1.38.31-64bit
RUN cd /tmp/emsdk && ./emsdk activate sdk-tag-1.38.31-64bit
RUN cd /tmp/emsdk && ./emsdk_env.sh
RUN export PATH=$PATH:/tmp/emsdk/emscripten/tag-1.38.31

# entrypoint
COPY ./entrypoint.sh /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

WORKDIR /tmp/workspace
