#
# Simple makefile for ts-lib project
#
# Copyright © 2018 All rights reserved for skullanbones™
#
# Strictly Confidential - Do not duplicate or distribute without written
# permission from skullanbones™ and authors

## Project
COMPONENT_NAME ?= ts-lib
export PROJ_ROOT := $(CURDIR)
SUBDIRS = tests
SRCDIR = $(PROJ_ROOT)/src
BUILDDIR = $(PROJ_ROOT)/build
INCDIR = $(PROJ_ROOT)/include
export INCDIR

## Docker
DOCKER_IMAGE_VER ?= v2
DOCKER_IMAGE_NAME ?= heliconwave/circleci
DOCKER_USER_ID ?= $(USER)

## Compiler
CXX = g++
STATIC = libts.a
DYNAMIC = libts.so
CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11 -fPIC
LDFLAGS = -shared
# Only needed if linkage to libts.so
#export LD_LIBRARY_PATH=$(BUILDDIR):$LD_LIBRARY_PATH

## Python
PYTHON_VERSION ?= 3

## Sources
SRCS = 	TsParser.cc \
		GetBits.cc \
		TsDemuxer.cc \
		TsStatistics.cc

HDRS = 	include/GetBits.h \
		include/TsDemuxer.h \
		include/TsPacketInfo.h \
		include/TsParser.h \
		include/TsStandards.h \
		include/TsStatistics.h

OBJS = $(patsubst %.cc,$(BUILDDIR)/%.o,$(SRCS))

$(info $$OBJS is $(OBJS))

## Commands
docker_command = docker run --env CXX="$(CXX)" --env CXXFLAGS="$(CXXFLAGS)" \
					--env LOCAL_USER_ID=`id -u ${DOCKER_USER_ID}` \
 					--rm -v $$(pwd):/tmp/workspace \
 					--workdir /tmp/workspace \
 					$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) \
 					make $1

.PHONY: all clean lint flake docker-image docker-bash test gtests run clang unit-test component_tests

help:
	@echo
	@echo '  all                   - build and create tsparser main executable.'
	@echo '  lint                  - run clang formating for c++ and flake8 for python'
	@echo '  flake                 - run flake8 on python files.'
	@echo '  clang                 - run clang-tidy on c++ files.'
	@echo '  run                   - run tsparser for bbc_one.ts asset and write elementary streams.'
	@echo '  docker-image          - builds new docker image with name:tag in Makefile.'
	@echo '  docker-bash           - starts a docker bash session with settings in makefile.'
	@echo '  tests                 - run all tests unit & component.'
	@echo '  unit-tests            - run all unit tests.'
	@echo '  gtest                 - execute gtest executable with unit test suite.'
	@echo '  env                   - build python virtual environment for pytest.'
	@echo '  component_tests       - run all component tests.'
	@echo '  so                    - make shared object as dynamic linkage library.'
	@echo '  clean                 - deletes build content.'
	@echo

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

$(BUILDDIR)/$(STATIC): $(OBJS)
	@echo "[Link (Static)]"
	@ar rcs $@ $^

so: $(BUILDDIR)/$(DYNAMIC)

$(BUILDDIR)/$(DYNAMIC): $(OBJS)
	@echo "[Link (Dynamic)]"
	$(CXX) ${LDFLAGS} -o $@ $^

lint: flake
	find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -exec clang-format-5.0 -style=file -i {} \;

flake:
	flake8 component_tests

clang:
	clang-tidy-5.0 src/*.cc -checks=* -- -std=c++11 -I/usr/include/c++/5/ -I./include

run: $(BUILDDIR)/tsparser
	cat $(PROJ_ROOT)/assets/bbc_one.ts | $(BUILDDIR)/tsparser --info 258 --write 2304 --write 2305 --write 2306 --write 2342

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

tests: unit-tests component_tests

### unit tests

build-unit-tests: $(BUILDDIR)/$(STATIC)
	docker pull $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER)
	$(call docker_command, gtests)

unit-tests: build-unit-tests
	@echo "[Running unit tests..]"
	$(MAKE) -C tests unit-tests

gtests:
	$(MAKE) -C tests gtests

run-gtests:
	$(MAKE) -C tests unit-tests

### component tests

env:
	virtualenv -p python$(PYTHON_VERSION) $@
	./env/bin/pip install -r component_tests/requirements.txt

component_tests: env $(BUILDDIR)/tsparser
	@echo "[Running component tests..]"
	./env/bin/pytest

clean:
	rm -f $(OBJS)
	rm -f $(BUILDDIR)/tsparser
	rm -f $(BUILDDIR)/main.o
	rm -f $(BUILDDIR)/$(STATIC)
	rm -f $(BUILDDIR)/$(DYNAMIC)
	rm -rf env/
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
