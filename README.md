# cpcos
CPCOS - **C**ross-**P**latform **C**++ **O**pen-source **S**tarter Project

This project aims to benifit C++ developers by providing a ready-to-start environment which
1. integrates popular open-source libraries with sample code
2. is able to compile under the 3 main PC platform - Windows, Linux (Ubuntu) and MacOS
3. can be cross-compiled for embedded platform (i.e. embedded Linux for arm) within a docker container

## Requirements
  1. conan
  2. docker (optional, for cross-compile only)

## Quick Start
- build for local machine (tested on OSX and Linux only)
  ```$ ./build.sh```
- cross build for {i386, arm32v7} linux
  1. create docker container for target environment (for 1st time only)
  ```$ ./create_docker_image_arm32v7.sh  # arm (ex. rpi)```
  ```$ ./create_docker_image_i386.sh  # i386 ```
  2. enter docker environment
  ```$ ./run_docker_arm32v7.sh  # arm```
  ```$ ./run_docker_i386.sh  # i386```
  3. build the project just like local environment
  ```$ ./build.sh```
- (optional) apply [pre-commit](https://pre-commit.com/)
  - install ```pre-commit``` (see [instructions](https://pre-commit.com/#install))
  - setup the project (only required once)
  ```$ pre-commit install```

## Introduction
Software development with C and C++ usually requires 3rd-party libraries, which save developers a lot of time.

However, playing with 3rd-party libraries is still painful since there're too many choices for the same feature. To evaluate a 3rd-party library, developers should read the documents, sample code and better to test it before introduce into current project -- the evaluation process takes a lot of effort and time.

Furthermore, if multi-platform support (or cross-compile) is required, the dependency issue increases drastically along with the number of introducesd libraries -- sloving it will be a nightmare.

That's the reason I started this project, and hopefully it will reduce some development overhead for others and we can spend more time on software design and coding.
