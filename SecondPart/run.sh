#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
# valgrind --tool=callgrind ${DIR}/build/release/Driver
# valgrind --tool=massif ${DIR}/build/release/Driver
# valgrind --leak-check=full --show-leak-kinds=all ${DIR}/build/release/Driver
${DIR}/build/release/Driver
