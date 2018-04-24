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
BUILDDIR = $(PROJ_ROOT)/build
INCDIR = $(PROJ_ROOT)/include
export INCDIR
PYTHON_VERSION ?= 3


SRCS = TsParser.cc GetBits.cc TsDemuxer.cc TsStatistics.cc
HDRS = include/GetBits.h include/GetBits.hh include/TsDemuxer.h \
		include/TsPacketInfo.h include/TsParser.h include/TsStandards.h \
		include/TsStatistics.h
OBJS = $(patsubst %.cc,$(BUILDDIR)/%.o,$(SRCS))

$(info $$OBJS is $(OBJS))

docker_command = docker run -e CXX="$(CXX)" -e CXXFLAGS="$(CXXFLAGS)" --rm -v $$(pwd):/tmp/workspace -w /tmp/workspace $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) make $1

.PHONY: all clean lint flake docker-image docker-bash test gtests run clang unit-test component_tests

all: $(BUILDDIR) $(BUILDDIR)/tsparser

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/tsparser: $(BUILDDIR)/main.o $(BUILDDIR)/$(STATIC) $(HDRS)
	$(CXX) -o $@ $(BUILDDIR)/main.o -L$(BUILDDIR) -lts

$(BUILDDIR)/main.o: $(SRCDIR)/main.cc $(HDRS)
	$(CXX) -o $@ -I$(INCDIR) -c $(CXXFLAGS) $(SRCDIR)/main.cc

$(OBJS): $(BUILDDIR)/%.o : $(SRCDIR)/%.cc
	@echo [Compile] $<
	@$(CXX) -I$(INCDIR) -c $(CXXFLAGS) $< -o $@

$(BUILDDIR)/$(STATIC): $(OBJS) $(HDRS)
	@echo "[Link (Static)]"
	@ar rcs $@ $^

lint: flake
	find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -exec clang-format-5.0 -style=file -i {} \;

flake:
	flake8 component_tests

clang:
	clang-tidy-5.0 src/*.cc -checks=* -- -std=c++11 -I/usr/include/c++/5/ -I./include

run: $(BUILDDIR)/tsparser
	cat bbc_one.ts | $(BUILDDIR)/tsparser --info 258 --write 2304 --write 2305 --write 2306 --write 2342

### docker stuff

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

### all tests

test: unit-test component_tests

### unit tests

unit-test: $(BUILDDIR)/$(STATIC)
	@echo "[Running unit tests..]"
	docker pull $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER)
	$(call docker_command, gtests)

gtests:
	$(MAKE) -C tests

### component tests

env:
	virtualenv -p python$(PYTHON_VERSION) $@
	./env/bin/pip install -r component_tests/requirements.txt

component_tests: env tsparser
	@echo "[Running component tests..]"
	./env/bin/pytest

clean:
	rm -f $(OBJS)
	rm -f $(BUILDDIR)/tsparser
	rm -f $(BUILDDIR)/main.o
	rm -f $(BUILDDIR)/$(STATIC)
	rm -rf env/
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
