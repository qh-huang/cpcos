#!/usr/bin/env bash

# toggle to debug the script
# options could be found by 'set help'
set -o errexit
#set -o xtrace
#set -o verbose

#### global variables ####
if [ "$(uname)" == "Darwin" ]; then
    ABSOLUTE_FILENAME=`readlink -f "$0"`    
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    ABSOLUTE_FILENAME=`readlink -e "$0"`
else
    echo "Unsupported OS"
    exit 1
fi
PROJECT_ROOT=$(dirname ${ABSOLUTE_FILENAME})

BUILD_DIR=${PROJECT_ROOT}/build_"$(uname -m)"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
conan install ${PROJECT_ROOT} --build=missing
cmake -S ${PROJECT_ROOT} -B ${BUILD_DIR}
cmake --build ${BUILD_DIR}
cd ${PROJECT_ROOT}
