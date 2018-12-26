#
# Simple Dockerfile for building and debugging ts-lib with necessary tools.
#
# Copyright © 2018 All rights reserved for skullanbones™
#

FROM ubuntu:16.04 as base

### Create user account ###
FROM base as user
RUN apt-get update && apt-get -y --no-install-recommends install \
    ca-certificates=20170717~16.04.1 \
    curl=7.47.0-1ubuntu2.11

RUN gpg --keyserver ha.pool.sks-keyservers.net --recv-keys B42F6819007F00F88E364FD4036A9C25BF357DD4
RUN curl -o /usr/local/bin/gosu -SL "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture)" \
    && curl -o /usr/local/bin/gosu.asc -SL "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture).asc" \
    && gpg --verify /usr/local/bin/gosu.asc \
    && rm /usr/local/bin/gosu.asc \
    && chmod +x /usr/local/bin/gosu

### Builder image ###
FROM user as build

RUN apt-get update && apt-get install --yes --no-install-recommends
RUN apt-get install --yes --no-install-recommends software-properties-common=0.96.20.7
RUN apt-get install --yes --no-install-recommends build-essential=12.1ubuntu2
RUN apt-get install --yes --no-install-recommends clang-format-5.0=1:5.0-3~16.04.1
RUN apt-get install --yes --no-install-recommends tstools=1.11-1ubuntu1
RUN apt-get install --yes --no-install-recommends gdb=7.11.1-0ubuntu1~16.5
RUN apt-get install --yes --no-install-recommends gdbserver=7.11.1-0ubuntu1~16.5
RUN apt-get install --yes --no-install-recommends python3-setuptools=20.7.0-1
RUN apt-get install --yes --no-install-recommends virtualenv=15.0.1+ds-3ubuntu1
RUN apt-get install --yes --no-install-recommends python3-pip=8.1.1-2ubuntu0.4
RUN apt-get install --yes --no-install-recommends python3-dev=3.5.1-3
RUN apt-get install --yes --no-install-recommends python3-virtualenv=15.0.1+ds-3ubuntu1
RUN apt-get install --yes --no-install-recommends wget=1.17.1-1ubuntu1.4
RUN apt-get install --yes --no-install-recommends lcov=1.12-2
RUN apt-get install --yes --no-install-recommends sudo=1.8.16-0ubuntu1.5

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

# entrypoint
COPY ./tools/entrypoint.sh /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

# Add root password
RUN echo "root:Docker!" | chpasswd

WORKDIR /tmp/workspace
