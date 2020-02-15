#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts Dockerfile
#
#    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
#
#    Unless you have obtained mpeg2ts under a different license,
#    this version of mpeg2ts is mpeg2ts|GPL.
#    Mpeg2ts|GPL is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as
#    published by the Free Software Foundation; either version 2,
#    or (at your option) any later version.
#
#    Mpeg2ts|GPL is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with mpeg2ts|GPL; see the file COPYING. If not, write to
#    the Free Software Foundation, 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.
#
#******************************************************************/

FROM ubuntu:18.04 as base


RUN apt-get update && apt-get --yes --no-install-recommends install \
    ca-certificates \
    curl \
    gnupg2 \
    gosu \
    apt-utils \
    software-properties-common \
    build-essential \
    wget \
    lcov \
    sudo \
    zip \
    unzip \
    doxygen \
    graphviz \
    cmake \
    git && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

### Builder image ###
FROM base as build

# LLVM Clang
ENV CLANG_VERSION=7
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-$CLANG_VERSION main" && \
apt-get update && \
apt-get install --yes --no-install-recommends clang-$CLANG_VERSION \
  clang-tidy-$CLANG_VERSION \
  clang-format-$CLANG_VERSION \
  llvm-$CLANG_VERSION-dev \
  libclang-$CLANG_VERSION-dev && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*


# python support
RUN apt-get update && \
  apt-get install --yes --no-install-recommends \
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
  && cmake .. && make -j $(nproc) && make install \
  && cd / && rm -rf /googletest

# Install benchmark
RUN git clone -q https://github.com/google/benchmark.git /benchmark \
  && cd benchmark \
  && git checkout tags/v1.4.0 \
  && mkdir -p /benchmark/build \
  && cd /benchmark/build \
  && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc) && make install \
  && cd / && rm -rf /benchmark

# Install latest iwyu
RUN git clone https://github.com/include-what-you-use/include-what-you-use.git /iwyu \
  && mkdir /iwyu/build \
  && cd /iwyu && git checkout clang_$CLANG_VERSION.0 \
  && cd /iwyu/build \
  && cmake .. && cmake --build . -- -j$(nproc) && make install \
  && cd / && rm -rf /iwyu

# Install as docker user (after entrypoint)
# Install Emscripten SDK/toolchain
ENV EMSDK_VERSION=1.39
ENV EMSDK_BUILD_VERSION=5
RUN git clone https://github.com/emscripten-core/emsdk.git /tmp/emsdk && \
cd /tmp/emsdk && \
./emsdk install $EMSDK_VERSION.$EMSDK_BUILD_VERSION && \
./emsdk activate $EMSDK_VERSION.$EMSDK_BUILD_VERSION
ENV PATH="${PATH}:/tmp/emsdk:/tmp/emsdk/upstream/emscripten:/tmp/emsdk/node/12.9.1_64bit/bin"
RUN echo "PATH: " $PATH
RUN em++ --version

# entrypoint
COPY ./entrypoint.sh /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

# WORKDIR /tmp/workspace
RUN adduser docker
