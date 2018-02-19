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
        git

WORKDIR /tmp/workspace