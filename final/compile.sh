#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd $DIR
mkdir -p dumpFiles
mkdir -p build/release
make clean
make -j4
