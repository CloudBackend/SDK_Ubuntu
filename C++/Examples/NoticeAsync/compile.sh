#!/bin/sh
# CloudBackend AB 2023-2025.
# compile.sh #
# version 2025-02-12
# This compiler script has sections corresponding to
# the different exercises in the Tutorial.
# Submit the number of the Exercise to compile.
# Example: sh compile.sh 1

if [ $# -gt 0 ]
then
    exe=$1
else
    exe="1"
fi
ARCH=`uname -m`
echo "identified computer architechture:" ${ARCH}
case "$ARCH" in
    "x86_64")
    COMPILER_COMMAND="g++ -std=c++23 -pthread "
    WARNINGS="-Wpedantic -Wextra -Weffc++ -Wsuggest-override -Wno-unused-parameter"
    # your path to the copy of the SDK binary lib
    # export LD_LIBRARY_PATH=${HOME}"/cbe/current/C++/lib/Linux_x86/"
    # libCBE=${HOME}"/cbe/current/C++/lib/Linux_x86/libcb_sdk.so"
    libCBE=${HOME}"/cbe/current/C++/lib/Linux_x86/libcb_sdk.a"
    ;;

    *)
    echo ${ARCH} "platform is not supported in this release"
    exit 1
    ;;
esac

echo "about to compile:" ${exe}
case "${exe}" in
    1)
    ${COMPILER_COMMAND} ${WARNINGS} -o noticeWrite Main-write.cpp Notice-write.cpp ${libCBE} -I ${HOME}"/cbe/current/C++/include"
    command_result=$?
    ;;
    
    2)
    ${COMPILER_COMMAND} ${WARNINGS} -o noticeListen Main-listen.cpp Notice-listen.cpp ${libCBE} -I ${HOME}"/cbe/current/C++/include"
    command_result=$?
    ;;
    
    *)
    # Default.
    echo "syntax : "$0" <1-2> "
    echo "example: "$0" 1"
    exit 2
    ;;
esac

if [ ${command_result} -eq 0 ]
then
    echo "to run use: sh run.sh" ${exe}
else
    echo "compilation had errors"
fi
