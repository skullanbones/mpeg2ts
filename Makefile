#
# Simple makefile for ts-lib project
#
# Copyright © 2018 All rights reserved for skullanbones™
#
# Strictly Confidential - Do not duplicate or distribute without written
# permission from skullanbones™ and authors

COMPONENT_NAME ?= ts-lib
DOCKER_IMAGE_VER ?= v1
DOCKER_IMAGE_NAME ?= tslib-docker-image
export PROJ_ROOT := $(CURDIR)
SUBDIRS = tests
CXX = g++
STATIC = libts.a
CXXFLAGS = -Wall -Winline -pipe -std=c++11
SRCDIR = $(PROJ_ROOT)/src
INCDIR = $(PROJ_ROOT)/include
export INCDIR


SRCS = src/TsParser.cc src/GetBits.cc src/TsDemuxer.cc
OBJS = $(SRCS:.cc=.o)

docker_command = docker run --rm -v $$(pwd):/tmp/workspace -w /tmp/workspace $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) make $1

.PHONY: all clean lint docker-image docker-bash

all: tsparser

tsparser: main.o $(OBJS) $(STATIC)
	$(CXX) -o $@ main.o -L. -lts

main.o: $(SRCDIR)/main.cc
	$(CXX) -I$(INCDIR) -c $(CXXFLAGS) $(SRCDIR)/main.cc

.cc.o:
	@echo [Compile] $<
	@$(CXX) -I$(INCDIR) -c $(CXXFLAGS) $< -o $@

$(STATIC): $(OBJS)
	@echo "[Link (Static)]"
	@ar rcs $@ $^

lint:
	find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format-5.0 -style=file -i {} \;

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

# TODO check that docker-image exists...
test: $(STATIC)
	$(call docker_command, gtests)

gtests:
	$(MAKE) -C tests

clean:
	rm -f $(OBJS)
	rm tsparser
	$(MAKE) -C $(SUBDIRS) clean
