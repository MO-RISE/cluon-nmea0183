# cluon-nmea0183

Copyright 2021 RISE Research Institute of Sweden - Maritime Operations. Licensed under the Apache License Version 2.0. For details, please contact Fredrik Olsson (fredrik.x.olsson(at)ri.se).

A [libcluon](https://github.com/chrberger/libcluon)-based microservice for eavesdropping on a NMEA0183 stream over either UDP or TCP. This software does not perform any parsing or validation of the NMEA0183 sentences, merely acts as a one-way bridge to a libcluon group.

## How do I get it?
Each release of `cluon-nmea0183` is published as a docker image [here](https://github.com/orgs/MO-RISE/packages/container/package/cluon-nmea0183) and is publicly available.

Can also be used as a standalone commandline tool. No pre-built binaries are, however, provided for this purpose.

## Example docker-compose setup
The example below showcases a setup with two listeners (listening on two separate streams (one UDP and one TCP)).
```yaml
version: '2'
services:    
    listener_1:
        container_name: cluon-nmea0183-listener-1
        image: ghcr.io/mo-rise/cluon-nmea0183:v0.5.0
        restart: on-failure
        network_mode: "host"
        command: "--cid 111 --id 1 --udp -a 255.255.255.255 -p 1456"
    listener_2:
        container_name: cluon-nmea0183-listener-2
        image: ghcr.io/mo-rise/cluon-nmea0183:v0.5.0
        restart: on-failure
        network_mode: "host"
        command: "--cid 111 --id 2 -a 171.31.16.42 -p 6002"
```

## Details

### Message set
Makes use of the public message set for maritime applications: https://github.com/MO-RISE/memo

### Build from source
This repository makes use of [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) for dependency resolution as an interal part of the CMake setup. As a result, the only requirements for building from source are:
* a C++17 compliant compiler
* CMake (>=3.14)

As part of the CMake configuration step, the following dependencies are downloaded and configured:
* [libcluon](https://github.com/chrberger/libcluon)
* [CLI11](https://github.com/CLIUtils/CLI11)
* [doctest](https://github.com/onqtam/doctest) (for testing only)

To build (from the root directory of this repo):
```cmd
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build -- -j 8
```

### Tests

Unit tests for the assembler is compiled into the executable `cluon-nmea0183-tester`.

To run tests (after successful build):
```cmd
cd build
ctest -C Debug -T test --output-on-failure --

### Development setup
This repo contains some configuration files (in the `.vscode`-folder) for getting started easy on the following setup:
* Ubuntu 20.04 (WSL2 is fine)
* GCC 9
* python 3
* VSCode as IDE, using the following extensions:
  - C/C++ (ms-vscode.cpptools)
  - C/C++ Extension Pack (ms-vscode.cpptools-extension-pack)
  - CMake Tools (ms-vscode.cmake-tools)
  - Python (ms-python.python)

Do the following steps to get started:
* Clone repo
* Create a python virtual environment (`python3 -m venv venv`) in the root of the repo.
* Open vscode in the repo root (`code .`)

The provided configuration is very lightweight and should be easily adaptable to other enviroments/setups.

