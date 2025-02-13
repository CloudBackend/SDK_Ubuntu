#!/usr/bin/sh
# compile.sh
# release 2025-02-12

PARENTSCRIPT_PATH="$(dirname "$0")"
cd ${PARENTSCRIPT_PATH}
echo ${PWD}

if [ $# -gt 0 ]
then
    exercise=$1
else
    exercise="1"
fi
ARCH=`uname -m`
echo "computer architechture ${ARCH}"
case "${ARCH}" in
    "x86_64")
    COMPILER_COMMAND="g++ -std=c++11 -pthread -ggdb3"
    # libCBE=${HOME}"/cbe/current/C++/lib/Linux_x86/libcb_sdk.so"
    libCBE=${HOME}"/cbe/current/C++/lib/Linux_x86/libcb_sdk.a"
    CODE_PATH="./"
    ;;

    *)
    uname -a
    echo "platform not supported in this release"
    exit 1
    ;;
esac

case "$exercise" in
  "1")
    echo "compile example code."
     ${COMPILER_COMMAND} -o "logintest" "${CODE_PATH}Logintest.cpp" ${libCBE} -I "/home/anders/cbe/current/C++/include" 
    if [ $? -eq 0 ]
    then
        echo "To run use: sh run.sh"
    else
        echo "Error encountered."
    fi
    ;;

    *)
    # Default.
    echo "syntax: "$0" <1-7> "
    ;;
esac
