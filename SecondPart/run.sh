#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
# valgrind --tool=callgrind ${DIR}/build/release/Driver
# valgrind --leak-check=full ${DIR}/build/release/Driver
${DIR}/build/release/Driver
