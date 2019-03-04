#!/bin/bash

# get git root
get_repo_root(){
    local REPO_ROOT="$(git rev-parse --show-toplevel)"
    echo "$REPO_ROOT"
}

# get sub directory in source three
get_sub_dir() {
    REPO_ROOT=$(get_repo_root)
    CUR_DIR=$(pwd)
    SUB_DIR=$(echo "$CUR_DIR" | grep -oP "^$REPO_ROOT\K.*")
    echo "$SUB_DIR"
}

# Run commands inside docker container
docker_run() {
    REPO_ROOT=$(get_repo_root)
    echo "Using REPO_ROOT: " "$REPO_ROOT"
    source $REPO_ROOT/Makefile.variables

    SUB_DIR=$(get_sub_dir)
    echo "SUB_DIR: " "$SUB_DIR"

    echo "Starting container: " "$DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VER"

    echo "Got command: " "$*"
    USER_ID=$(id -u $USER)
    echo "Using USER_ID:" $USER_ID

    docker run  --env LOCAL_USER_ID=$USER_ID \
                --rm \
                --volume $REPO_ROOT:/tmp/workspace \
		        --workdir /tmp/workspace$SUB_DIR \
                --env "TERM=xterm-256color" \
                --tty \
                --entrypoint /tmp/workspace/docker/entrypoint.sh \
				"$DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VER" \
				$*
}

# start tty session inside docker container
docker-interactive() {
    REPO_ROOT=$(get_repo_root)
    echo "Using REPO_ROOT: " "$REPO_ROOT"
    source $REPO_ROOT/Makefile.variables

    SUB_DIR=$(get_sub_dir)
    echo "SUB_DIR: " "$SUB_DIR"

    echo "Starting container: " "$DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VER"

    echo "Got command: " "$*"
    USER_ID=$(id -u $USER)
    echo "Using USER_ID:" $USER_ID

    docker run  --env LOCAL_USER_ID=$USER_ID \
                --rm \
                --interactive \
                --volume $REPO_ROOT:/tmp/workspace \
		        --workdir /tmp/workspace$SUB_DIR \
                --env "TERM=xterm-256color" \
                --tty \
                --entrypoint /tmp/workspace/docker/entrypoint.sh \
				"$DOCKER_IMAGE_NAME:$DOCKER_IMAGE_VER" /bin/bash
}

# Run make target inside docker
docker-make() {
    docker_run make "${@}"
}

# Run bash command inside docker
docker-bash() {
    docker_run "${@}"
}
