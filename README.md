# ts-lib
Make video great again!

## How to run it

```
make
cat test1.ts | ./tsparser
```

## Docker image
To build the image:
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