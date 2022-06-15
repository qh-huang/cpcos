#!/usr/bin/env bash

# toggle to debug the script
# options could be found by 'set help'
set -o errexit
#set -o xtrace
#set -o verbose

#### global variables ####
ABSOLUTE_FILENAME=`readlink -f "$0"`
PROJECT_ROOT=$(dirname ${ABSOLUTE_FILENAME})

docker run  -it \
            --platform linux/arm \
            -v${PROJECT_ROOT}:/root/cpcos \
            --workdir="/root/cpcos" \
            --rm \
            cpcos_arm32v7 \
            /bin/bash