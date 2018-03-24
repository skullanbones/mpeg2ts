#
# Simple makefile for ts-lib project
#
# Copyright © 2018 All rights reserved for skullanbones™
#
# Strictly Confidential - Do not duplicate or distribute without written
# permission from skullanbones™ and authors

COMPONENT_NAME ?= ts-lib
DOCKER_IMAGE_VER ?= v1
DOCKER_IMAGE_NAME ?= heliconwave/circleci
export PROJ_ROOT := $(CURDIR)
SUBDIRS = tests
CXX = g++
STATIC = libts.a
CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11
SRCDIR = $(PROJ_ROOT)/src
INCDIR = $(PROJ_ROOT)/include
export INCDIR


SRCS = src/TsParser.cc src/GetBits.cc src/TsDemuxer.cc
HDRS = include/GetBits.h include/GetBits.hh include/TsDemuxer.h \
		include/TsPacketInfo.h include/TsParser.h include/TsStandards.h
OBJS = $(SRCS:.cc=.o)

docker_command = docker run -e CXX="$(CXX)" -e CXXFLAGS="$(CXXFLAGS)" --rm -v $$(pwd):/tmp/workspace -w /tmp/workspace $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) make $1

.PHONY: all clean lint docker-image docker-bash test gtests run clang

all: tsparser

tsparser: main.o $(STATIC) $(HDRS)
	$(CXX) -o $@ main.o -L. -lts

main.o: $(SRCDIR)/main.cc $(HDRS)
	$(CXX) -I$(INCDIR) -c $(CXXFLAGS) $(SRCDIR)/main.cc

.cc.o:
	@echo [Compile] $<
	@$(CXX) -I$(INCDIR) -c $(CXXFLAGS) $< -o $@

$(STATIC): $(OBJS) $(HDRS)
	@echo "[Link (Static)]"
	@ar rcs $@ $^

lint:
	find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -exec clang-format-5.0 -style=file -i {} \;

clang:
	clang-tidy-5.0 -checks='*' -header-filter="^include" -p . src/*.cc

run: tsparser
	cat bbc_one.ts | ./tsparser --info 258 --write 2304 --write 2305 --write 2306 --write 2342

docker-image:
	docker build \
		--file=Dockerfile \
		--tag=$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) .

docker-bash:
	docker run \
		--rm \
		--interactive \
		--tty=true \
		--volume=$$(pwd):/tmp/workspace \
		$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) /bin/bash

test: $(STATIC)
	docker pull $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER)
	$(call docker_command, gtests)

gtests:
	$(MAKE) -C tests

clean:
	rm -f $(OBJS)
	rm -f tsparser
	rm -f main.o
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
