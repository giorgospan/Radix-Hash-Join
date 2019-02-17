#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

WORKLOAD_DIR=${1-$DIR/workloads/small}
WORKLOAD_DIR=$(echo $WORKLOAD_DIR | sed 's:/*$::')

cd $WORKLOAD_DIR

WORKLOAD=$(basename "$PWD")
echo execute $WORKLOAD ...
$DIR/build/release/harness *.init *.work *.result ../../run.sh
