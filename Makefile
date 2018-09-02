#
# Simple makefile for ts-lib project
#
# Copyright © 2018 All rights reserved for skullanbones™
#
# Strictly Confidential - Do not duplicate or distribute without written
# permission from skullanbones™ and authors

## Project
COMPONENT_NAME ?= mp2ts
export PROJ_ROOT := $(CURDIR)
SUBDIRS = tests
SRCDIR = $(PROJ_ROOT)/src
BUILDDIR = $(PROJ_ROOT)/build
TOOLSDIR = $(PROJ_ROOT)/tools
3RDPARTYDIR = $(PROJ_ROOT)/3rd-party

## 3rd-party settings
PLOG_VERSION=1.1.4

INCLUDE_DIRS += -I$(PROJ_ROOT)/include \
				-I$(3RDPARTYDIR)/plog-$(PLOG_VERSION)/include

export INCLUDE_DIRS
BUILD_TYPE ?= DEBUG

## Machine
CORES ?= $(shell nproc)
MAKEFLAGS+="-j $(CORES)"
$(info MAKEFLAGS= $(MAKEFLAGS))

## Docker
DOCKER_IMAGE_VER ?= v4
DOCKER_IMAGE_NAME ?= heliconwave/circleci
DOCKER_USER_ID ?= $(USER)

## Compiler
CXX = g++
STATIC = lib$(COMPONENT_NAME).a
DYNAMIC = lib$(COMPONENT_NAME).so

CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11 -fPIC
ifeq ($(BUILD_TYPE),DEBUG)
	CXXFLAGS += -g -O0
else ifeq ($(BUILD_TYPE),RELEASE)
	CXXFLAGS += -O3
endif
LDFLAGS = -shared
# Only needed if linkage to libts.so
#export LD_LIBRARY_PATH=$(BUILDDIR):$LD_LIBRARY_PATH

## Python
PYTHON_VERSION ?= 3

## Sources
SRCS = 	TsParser.cc \
		GetBits.cc \
		TsDemuxer.cc \
		TsStatistics.cc \
        mpeg2vid/Mpeg2VideoParser.cc \
        h264/H264Parser.cc \
        PesPacket.cc \
        PsiTables.cc \
        TsPacketInfo.cc

HDRS = 	include/public/mp2ts.h \
		include/public/Ts_IEC13818-1.h \
		include/GetBits.h \
		include/TsParser.h \
		include/mpeg2vid/Mpeg2VideoParser.h \
		include/h264/H264Parser.h \
		include/mpeg2vid/Mpeg2VideoParser.h

OBJS = $(patsubst %.cc,$(BUILDDIR)/%.o,$(SRCS))

$(info $$OBJS is $(OBJS))

## Commands
docker_command = docker run --env CXX="$(CXX)" --env CXXFLAGS="$(CXXFLAGS)" \
					--env LOCAL_USER_ID=`id -u ${DOCKER_USER_ID}` \
 					--rm -v $$(pwd):/tmp/workspace \
 					--workdir /tmp/workspace \
 					$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) \
 					make $1

docker_run = docker run \
				--rm \
				--interactive \
				--tty=true \
				--volume=$$(pwd):/tmp/workspace \
				--workdir /tmp/workspace \
				--env LOCAL_USER_ID=`id -u ${DOCKER_USER_ID}` \
				$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) /bin/bash -c $1

.PHONY: all clean lint flake docker-image docker-bash test gtests run clang-tidy clang-format unit-test component-tests

help:
	@echo
	@echo '  all                   - build and create tsparser main executable.'
	@echo '  lint                  - run clang formating for c++ and flake8 for python'
	@echo '  flake                 - run flake8 on python files.'
	@echo '  clang-tidy            - run clang-tidy on c++ files.'
	@echo '  clang-format          - run clang-format on c++ files following rules specified in tools dir.'
	@echo '  run                   - run tsparser for bbc_one.ts asset and write elementary streams.'
	@echo '  docker-image          - builds new docker image with name:tag in Makefile.'
	@echo '  docker-bash           - starts a docker bash session with settings in makefile.'
	@echo '  tests                 - run all tests unit & component.'
	@echo '  unit-tests            - run all unit tests.'
	@echo '  gtest                 - execute gtest executable with unit test suite.'
	@echo '  env                   - build python virtual environment for pytest.'
	@echo '  component-tests       - run all component tests.'
	@echo '  libs                  - make both static and dynamic libs.'
	@echo '  shared                - make static object as static linkage library.'
	@echo '  static                - make shared object as dynamic linkage library.'
	@echo '  3rd-party             - install 3rd-party dependencies.'
	@echo '  plog                  - install 3rd-party plog logging library.'
	@echo '  clean                 - deletes build content.'
	@echo '  clean-all             - deletes build content + downloaded 3rd-party.'
	@echo

all: $(BUILDDIR) $(BUILDDIR)/tsparser

$(BUILDDIR):
	mkdir -p $(BUILDDIR)
	mkdir -p $(BUILDDIR)/mpeg2vid
	mkdir -p $(BUILDDIR)/h264

$(BUILDDIR)/tsparser: $(BUILDDIR)/main.o static $(HDRS)
	$(CXX) -o $@ $(BUILDDIR)/main.o -L$(BUILDDIR) -l$(COMPONENT_NAME)

$(BUILDDIR)/main.o: plog $(SRCDIR)/main.cc $(HDRS)
	$(CXX) -o $@ $(INCLUDE_DIRS) -c $(CXXFLAGS) $(SRCDIR)/main.cc

$(OBJS): $(BUILDDIR)/%.o : $(SRCDIR)/%.cc plog
	@echo [Compile] $<
	@$(CXX) $(INCLUDE_DIRS) -c $(CXXFLAGS) $< -o $@

libs: $(BUILDDIR) static shared

static: $(BUILDDIR)/$(STATIC)

$(BUILDDIR)/$(STATIC): $(OBJS)
	@echo "[Link (Static)]"
	@ar rcs $@ $^

shared: $(BUILDDIR)/$(DYNAMIC)

$(BUILDDIR)/$(DYNAMIC): $(OBJS)
	@echo "[Link (Dynamic)]"
	$(CXX) ${LDFLAGS} -o $@ $^

lint: flake clang-format

flake:
	flake8 component_tests

clang-format:
	find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -exec clang-format-5.0 -style=file -i {} \;

clang-tidy:
	clang-tidy-5.0 src/*.cc -checks=* -- -std=c++11 -I/usr/include/c++/5/ -I./include

run: $(BUILDDIR)/tsparser
	$(BUILDDIR)/tsparser --input $(PROJ_ROOT)/assets/bbc_one.ts --pid 258 --write 2304 --write 2305 --write 2306 --write 2342

### docker stuff

docker-image:
	docker build \
		--file=$(TOOLSDIR)/Dockerfile \
		--tag=$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) .

docker-bash:
	docker run \
		--rm \
		--interactive \
		--tty=true \
		--volume=$$(pwd):/tmp/workspace \
		--env LOCAL_USER_ID=`id -u ${DOCKER_USER_ID}` \
		$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) /bin/bash

### all tests

tests: unit-tests component-tests

### unit tests

build-unit-tests: static
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

component-tests: env $(BUILDDIR)/tsparser
	@echo "[Running component tests..]"
	./env/bin/pytest

### 3rd-party stuff
$(3RDPARTYDIR)/plog-$(PLOG_VERSION).tar.gz:
	wget https://github.com/SergiusTheBest/plog/archive/$(PLOG_VERSION).tar.gz -O $(3RDPARTYDIR)/plog-$(PLOG_VERSION).tar.gz

$(3RDPARTYDIR)/.plog_extracted: $(3RDPARTYDIR)/plog-$(PLOG_VERSION).tar.gz
	cd $(3RDPARTYDIR)
	tar xvf $(3RDPARTYDIR)/plog-$(PLOG_VERSION).tar.gz -C $(3RDPARTYDIR)
	touch $@

3rd-party: plog

plog: $(3RDPARTYDIR)/.plog_extracted

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

### Will force clean download cache & build directory
clean-all: clean
	rm -f $(3RDPARTYDIR)/plog-$(PLOG_VERSION).tar.gz
	rm -f $(3RDPARTYDIR)/.plog_extracted
	rm -rf $(3RDPARTYDIR)/plog-$(PLOG_VERSION)
	rm -rf $(BUILDDIR)

