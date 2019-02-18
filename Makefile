#
# Simple makefile for ts-lib project
#
# Copyright © 2018 All rights reserved for skullanbones™
#
# Strictly Confidential - Do not duplicate or distribute without written
# permission from skullanbones™ and authors

# Get variables
include Makefile.variables

## Project
COMPONENT_NAME ?= mpeg2ts
export PROJ_ROOT := $(CURDIR)
SUBDIRS = tests
SRCDIR = $(PROJ_ROOT)/src
BUILDDIR = $(PROJ_ROOT)/build
TOOLSDIR = $(PROJ_ROOT)/tools


export INCLUDE_DIRS
BUILD_TYPE ?= RELEASE

## Machine
CORES ?= $(shell nproc)
MAKEFLAGS+="-j $(CORES)"
$(info MAKEFLAGS= $(MAKEFLAGS))

			
LDFLAGS =
ifeq ($(BUILD_TYPE),DEBUG)
	CXXFLAGS += -g -O0 --coverage
	LDFLAGS += -lgcov
else ifeq ($(BUILD_TYPE),RELEASE)
	CXXFLAGS += -O3
	LDFLAGS +=
endif

# Only needed if linkage to libts.so
#export LD_LIBRARY_PATH=$(BUILDDIR):$LD_LIBRARY_PATH

## Python
PYTHON_VERSION ?= 3

$(info OBJS is: $(OBJS))

.PHONY: all clean lint flake docker-image docker-bash test gtests run clang-tidy clang-format unit-test component-tests cppcheck

help:
	@echo
	@echo '  all                   - build and create tsparser main executable.'
	@echo '  lint                  - run clang formating for c++ and flake8 for python'
	@echo '  flake                 - run flake8 on python files.'
	@echo '  clang-tidy            - run clang-tidy on c++ files.'
	@echo '  clang-format          - run clang-format on c++ files following rules specified in tools dir.'
	@echo '  cppcheck              - run cppcheck on c++ files.'
	@echo '  run                   - run tsparser for bbc_one.ts asset and write elementary streams.'
	@echo '  docker-image          - builds new docker image with name:tag in Makefile.'
	@echo '  benchmark-tests       - run all benchmark tests.'
	@echo '  coverage              - run code coverage on unit-tests.'
	@echo '  clean                 - deletes build content.'
	@echo '  clean-all             - deletes build content + downloaded 3rd-party.'
	@echo

all: help

lint: flake clang-format

flake:
	flake8 component_tests

clang-format:
	find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\)' -exec clang-format-5.0 -style=file -i {} \;

clang-tidy:
	clang-tidy-6.0 src/*.cc -checks=* -- -std=c++11 -I/usr/include/c++/5/ -I./include

cppcheck:
	cppcheck --enable=all $(SRCDIR)

run: $(BUILDDIR)/tsparser
	$(BUILDDIR)/tsparser --input $(PROJ_ROOT)/assets/bbc_one.ts --pid 258 --write 2304 --write 2305 --write 2306 --write 2342

### docker stuff

# Build docker image
docker-image:
	docker build \
		--file=$(TOOLSDIR)/Dockerfile \
		--tag=$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) \
		--tag=$(DOCKER_IMAGE_NAME):latest .

### coverage	

coverage: build-unit-tests
	$(MAKE) -C tests coverage


benchmark-tests: env $(BUILDDIR)/tsparser
	@echo "[Running component tests..]"
	./env/bin/pytest --benchmark-enable --benchmark-only

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -f $(BUILDDIR)/*.gcno
	rm -f $(BUILDDIR)/*.gcda
	rm -rf $(BUILDDIR)/coverage
	rm -rf $(BUILDDIR)/h264
	rm -rf $(BUILDDIR)/mpeg2vid
	rm -f $(BUILDDIR)/coverage.info

### Will force clean download cache & build directory
clean-all: clean
	rm -rf $(BUILDDIR)
	rm -rf $(PROJ_ROOT)/component_tests/downloaded_files
