#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

WORKLOAD_DIR=${1-$DIR/workloads/small}
WORKLOAD_DIR=$(echo $WORKLOAD_DIR | sed 's:/*$::')

cd $WORKLOAD_DIR

WORKLOAD=$(basename "$PWD")
# valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $DIR/unittest
$DIR/unittest
