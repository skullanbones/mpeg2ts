#
# Simple makefile for ts-lib project
#
# Copyright © 2018 All rights reserved for skullanbones™
#
# Strictly Confidential - Do not duplicate or distribute without written
# permission from skullanbones™ and authors

COMPONENT_NAME ?= ts-lib

.PHONY: all clean

all:
	g++ main.cc TsParser.cc GetBits.cc -std=c++11 -o tsparser

lint:
	find . -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format-5.0 -style=file -i {} \;

clean:
	rm *.o
	rm tsparser
