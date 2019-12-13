#
# Simple makefile for mpeg2ts project
#
# Copyright © 2018 All rights reserved
#
# Do not duplicate or distribute without written permission from authors

DOCKER_IMAGE_BASE=ubuntu_18_04
DOCKER_IMAGE_NAME=heliconwave/circleci
DOCKER_IMAGE_VER=v19


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
