#
# Simple Dockerfile for building and debugging ts-lib with necessary tools.
#
# Copyright © 2018 All rights reserved for skullanbones™
#

FROM ubuntu:16.04
MAINTAINER heliconwave <heliconwave@gmail.com>


RUN apt-get update && apt-get install --yes \
        software-properties-common \
        build-essential \
        clang-format-5.0 \
        tstools \
        git \
        gdb \
        gdbserver \
        cmake

COPY ./3rd-party/gtest/release-1.8.0.tar.gz /tmp
WORKDIR /tmp
RUN tar xvzf release-1.8.0.tar.gz
WORKDIR googletest-release-1.8.0/
RUN cmake -DBUILD_SHARED_LIBS=ON .
RUN make
RUN cp -a googletest/include/gtest /usr/include
RUN cp -a googlemock/include/gmock /usr/include
RUN cp -a googlemock/gtest/libgtest_main.so googlemock/gtest/libgtest.so /usr/lib/
WORKDIR /tmp/workspace
