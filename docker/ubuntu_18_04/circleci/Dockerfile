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
    cmake \
    git \
    valgrind && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

### Builder image ###
FROM base as build

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


# Install as docker user (after entrypoint)
# Install Emscripten SDK/toolchain
# ENV EMSDK_VERSION=1.38
# ENV EMSDK_BUILD_VERSION=31
# RUN git clone https://github.com/emscripten-core/emsdk.git /tmp/emsdk && \
# cd /tmp/emsdk && \
# ./emsdk install sdk-tag-$EMSDK_VERSION.$EMSDK_BUILD_VERSION-64bit && \
# ./emsdk activate sdk-tag-$EMSDK_VERSION.$EMSDK_BUILD_VERSION-64bit
# ENV PATH="${PATH}:/tmp/emsdk/emscripten/tag-$EMSDK_VERSION.$EMSDK_BUILD_VERSION"
# RUN echo "PATH: " $PATH
# RUN em++ --version

# entrypoint
COPY ./entrypoint.sh /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

WORKDIR /tmp/workspace
