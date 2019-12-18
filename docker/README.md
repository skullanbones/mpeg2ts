# Docker

To virtualize the Application build time dependencies they have been collected inside a docker image following
docker best practises. You only need to remember to source the 
```
source docker/docker-commands.sh
```
and you will be ready to run commands inside the docker container like. Here is a list of all commands.

| Command              | Meaning                     |
|----------------------|-----------------------------|
| docker-bash          | run a bash command inside a docker container
| docker-make          | run a make command inside a docker container
| docker-interactive   | starts a bash session inside a docker container
| docker-run           | run a command inside a docker container

## Examples

configuring CMake:
```Bash
cd build/
docker-bash cmake ..
```
building
```Bash
docker-bash make -j $(nproc)
```
and testing:
```Bash
docker-bash unit-tests
```
for example.
If you want to run a custom bash command you can do it by:
```Bash
docker-bash make help
```
for instance. To get an interactive bash session type:
```Bash
docker-interactive
```
which will give you a docker shell:
```Bash
docker@48fefc2ad3cf:/tmp/workspace/build
```

### Docker image
To just use the latest image just pull from our private registry/repository @ DockerHub:
```
docker pull heliconwave/circleci:v1
```
To build the image your self:
```
make docker-image
```
