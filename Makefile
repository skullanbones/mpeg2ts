#
# Simple makefile for mpeg2ts project
#
# Copyright © 2018 All rights reserved
#
# Do not duplicate or distribute without written
# permission from and authors


## Project
COMPONENT_NAME ?= mpeg2ts
export PROJ_ROOT := $(CURDIR)
SUBDIRS = tests
SRCDIR = $(PROJ_ROOT)/src
BUILDDIR = $(PROJ_ROOT)/build


## Machine
CORES ?= $(shell nproc)
MAKEFLAGS+="-j $(CORES)"
$(info MAKEFLAGS= $(MAKEFLAGS))


.PHONY: all lint flake clang-tidy run docker-image coverage clean docker-bash  

help:
	@echo
	@echo '  all                   - build and create tsparser main executable.'
	@echo '  lint                  - run clang formating for c++ and flake8 for python'
	@echo '  flake                 - run flake8 on python files.'
	@echo '  clang-tidy            - run clang-tidy on c++ files.'
	@echo '  run                   - run tsparser for bbc_one.ts asset and write elementary streams.'
	@echo '  coverage              - run code coverage on unit-tests.'
	@echo '  clean                 - deletes build content.'
	@echo '  clean-all             - deletes build content + downloaded 3rd-party.'
	@echo

all: help

lint: flake

flake:
	flake8 component_tests

fix_style:
	autopep8 --in-place --aggressive component_tests/*.py

clang-tidy:
	clang-tidy-6.0 src/*.cc -checks=* -- -std=c++11 -I/usr/include/c++/5/ -I./include

run: $(BUILDDIR)/tsparser
	$(BUILDDIR)/tsparser --input $(PROJ_ROOT)/assets/bbc_one.ts --pid 258 --write 2304 --write 2305 --write 2306 --write 2342

### docker stuff

### coverage	

coverage: build-unit-tests
	$(MAKE) -C tests coverage

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
