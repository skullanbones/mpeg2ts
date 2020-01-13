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

FROM ubuntu:16.04 as base

### Create user account ###
FROM base as user
RUN apt-get update && apt-get -y --no-install-recommends install \
    ca-certificates=20170717~16.04.1 \
    curl

RUN gpg --keyserver ha.pool.sks-keyservers.net --recv-keys B42F6819007F00F88E364FD4036A9C25BF357DD4
RUN curl -o /usr/local/bin/gosu -SL "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture)" \
    && curl -o /usr/local/bin/gosu.asc -SL "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture).asc" \
    && gpg --verify /usr/local/bin/gosu.asc \
    && rm /usr/local/bin/gosu.asc \
    && chmod +x /usr/local/bin/gosu

### Builder image ###
FROM user as build

RUN apt-get update && apt-get --yes --no-install-recommends install \
  apt-utils \
  software-properties-common \
  build-essential \
  wget \
  sudo \
  zip \
  unzip \
  cmake \
  git \
  graphviz && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*

# Install gtest/gmock
RUN git clone -q https://github.com/google/googletest.git /googletest \
  && cd googletest \
  && git checkout tags/release-1.8.1 \
  && mkdir -p /googletest/build \
  && cd /googletest/build \
  && cmake .. && make -j 16 && make install \
  && cd / && rm -rf /googletest

# Install benchmark
RUN git clone -q https://github.com/google/benchmark.git /benchmark \
  && cd benchmark \
  && git checkout tags/v1.4.0 \
  && mkdir -p /benchmark/build \
  && cd /benchmark/build \
  && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j 16 && make install \
  && cd / && rm -rf /benchmark


# python support
RUN apt-get update && apt-get --yes --no-install-recommends install \
  python3-setuptools=20.7.0-1 \
  virtualenv=15.0.1+ds-3ubuntu1 \
  python3-pip=8.1.1-2ubuntu0.4 \
  python3-dev=3.5.1-3 \
  python3-virtualenv=15.0.1+ds-3ubuntu1 \
  python3-pip && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*


#python packages
RUN pip3 install flake8

# entrypoint
COPY ./entrypoint.sh /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

WORKDIR /tmp/workspace
