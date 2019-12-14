#
# Simple makefile for mpeg2ts project
#
# Copyright © 2018 All rights reserved
#
# Do not duplicate or distribute without written permission from authors

include Makefile.variables


## Project
COMPONENT_NAME ?= mpeg2ts
export PROJ_ROOT := $(CURDIR)

.PHONY: all docker-image

help:
	@echo
	@echo '  docker-image          - builds new docker image with name:tag in Makefile.'
	@echo

all: help

# Build docker image
docker-image:
	docker build \
		--file=$(DOCKER_IMAGE_BASE)/Dockerfile \
		--tag=$(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_VER) \
		--tag=$(DOCKER_IMAGE_NAME):latest ./
