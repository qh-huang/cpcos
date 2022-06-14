#!/usr/bin/env bash

# toggle to debug the script
# options could be found by 'set help'
set -o errexit
#set -o xtrace
#set -o verbose

OS_NAME="$(uname -s)"
MACH_TYPE="$(uname -m)"

#### global variables ####
if [ ${OS_NAME} == "Darwin" ]; then
    ABSOLUTE_FILENAME=`readlink -f "$0"`
    BUILD_VARIANT="darwin"
elif [ "$(expr substr ${OS_NAME} 1 5)" == "Linux" ]; then
    ABSOLUTE_FILENAME=`readlink -e "$0"`
    if [ ${MACH_TYPE} == "x86_64" ]; then
        BUILD_VARIANT="linux64"
    elif [ ${MACH_TYPE} == "i686" ]; then
        BUILD_VARIANT="linux32"
    elif [ ${MACH_TYPE} == "armv7l" ]; then
        BUILD_VARIANT="armv7l"
    else
        echo "Unsupported machine type: ${MACH_TYPE}"
        exit 1
    fi
else
    echo "Unsupported OS"
    exit 1
fi

PROJECT_ROOT=$(dirname ${ABSOLUTE_FILENAME})

BUILD_DIR=${PROJECT_ROOT}/build_"${BUILD_VARIANT}"
CONANFILE=${PROJECT_ROOT}/conanfile_"${BUILD_VARIANT}".txt

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
conan install ${CONANFILE} --build=missing
cmake -S ${PROJECT_ROOT} -B ${BUILD_DIR}
cmake --build ${BUILD_DIR}
cd ${BUILD_DIR} && ctest
cd ${PROJECT_ROOT}
