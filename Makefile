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

.PHONY: all clean lint docker-image docker-bash

all:
	g++ main.cc TsParser.cc GetBits.cc TsDemuxer.cc -std=c++11 -o tsparser

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

clean:
	rm *.o
	rm tsparser
