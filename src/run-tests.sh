#!/bin/bash

set -xe

cd ./tests
make clean
make
make vrun