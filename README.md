# ts-lib
Make video great again!

## How to run it

```
make
cat test1.ts | ./tsparser
```

## Docker image
To just use the latest image just pull from our private registry/repository @ DockerHub:
```
docker pull skullanbones/ts-lib:v1
```
If you get a security message like this:
```
Error response from daemon: pull access denied for heliconave/ts-lib, repository does not exist or may require 'docker login'
```
it means you must first login:
```
docker login
```
Ask for username/password from kohnech.
To build the image your self:
```
make docker-image
```
To spin-up a container:
```
make docker-bash
```
From here you can use the 3rd-party dependencies like
tstools, clang-format-5.0 etc...

## How to test it
In order to run all unit tests just type:
```
make test
```
This will spin up a docker container with gtest/gmock and execute all tests.