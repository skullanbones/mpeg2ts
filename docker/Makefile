#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts Makefile
#
#    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
#
#    Unless you have obtained mpeg2ts under a different license,
#    this version of mpeg2ts is mpeg2ts|GPL.
#    Mpeg2ts|GPL is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as
#    published by the Free Software Foundation; either version 2,
#    or (at your option) any later version.
#
#    Mpeg2ts|GPL is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with mpeg2ts|GPL; see the file COPYING. If not, write to
#    the Free Software Foundation, 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.
#
#******************************************************************/

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

docker-image-ubuntu-16.04:
	docker build \
		--file=ubuntu_16_04/Dockerfile \
		--tag=heliconwave/mpeg2ts-u16.04:v2 \
		--tag=heliconwave/mpeg2ts-u16.04:latest ./


docker-bash-ubuntu-16.04:
	docker run \
		--rm \
		--interactive \
		--tty=true \
		--env LOCAL_USER_ID=`id -u ${USER}` \
		--env "TERM=xterm-256color" \
		--volume=$$(pwd)/workspace:/home/docker/workspace \
		--workdir=/home/docker/workspace \
		heliconwave/mpeg2ts-u16.04:latest /bin/bash
