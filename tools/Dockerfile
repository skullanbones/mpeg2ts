#
# Simple Dockerfile for building and debugging ts-lib with necessary tools.
#
# Copyright © 2018 All rights reserved for skullanbones™
#

FROM ubuntu:16.04 as base

### Create user account ###
FROM base as user
RUN apt-get update && apt-get -y --no-install-recommends install \
    ca-certificates \
    curl

RUN gpg --keyserver ha.pool.sks-keyservers.net --recv-keys B42F6819007F00F88E364FD4036A9C25BF357DD4
RUN curl -o /usr/local/bin/gosu -SL "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture)" \
    && curl -o /usr/local/bin/gosu.asc -SL "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture).asc" \
    && gpg --verify /usr/local/bin/gosu.asc \
    && rm /usr/local/bin/gosu.asc \
    && chmod +x /usr/local/bin/gosu

### Builder image ###
FROM user as build

RUN apt-get update && apt-get install --yes --no-install-recommends
RUN apt-get install --yes --no-install-recommends software-properties-common
RUN apt-get install --yes --no-install-recommends build-essential
RUN apt-get install --yes --no-install-recommends clang-format-5.0
RUN apt-get install --yes --no-install-recommends tstools
RUN apt-get install --yes --no-install-recommends git
RUN apt-get install --yes --no-install-recommends gdb
RUN apt-get install --yes --no-install-recommends gdbserver
RUN apt-get install --yes --no-install-recommends python3-setuptools
RUN apt-get install --yes --no-install-recommends virtualenv
RUN apt-get install --yes --no-install-recommends python3-pip
RUN apt-get install --yes --no-install-recommends python3-dev
RUN apt-get install --yes --no-install-recommends python3-virtualenv
RUN apt-get install --yes --no-install-recommends wget
RUN apt-get install --yes --no-install-recommends lcov
RUN apt-get install --yes --no-install-recommends sudo

# Install latest cmake
RUN curl -o /cmake-3.12.4-Linux-x86_64.sh -SL "https://cmake.org/files/v3.12/cmake-3.12.4-Linux-x86_64.sh"
RUN mkdir /opt/cmake
RUN sh /cmake-3.12.4-Linux-x86_64.sh --prefix=/opt/cmake --skip-license
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
