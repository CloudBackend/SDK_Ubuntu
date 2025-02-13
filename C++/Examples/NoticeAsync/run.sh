#!/bin/sh
# run.sh #
# This run script has sections corresponding to the different exercises in the Tutorial on the website.
# Submit the number of the exercise to compile.
# example: sh run.sh 1

echo "CloudBackend SDK is provided under a limited evaluation licence that is not for production use."

if [ $# -gt 0 ]
then
  exe=$1
else
  exe="1"
fi
ARCH=`uname -m`
export LD_LIBRARY_PATH="../../lib/Linux_x86"
echo "computer architechture:" ${ARCH}
case "$exe" in
  1)
  app="noticeWrite"
  echo "run" ${app}
  ./${app}
  ;;

  2)
  app="noticeListen"
  echo "run" ${app}
  ./${app}
  ;;
  
  *)
  # Default.
  echo
  echo "syntax: "$0" <1-2> "
  echo "example: sh run.sh 1"
  exit 1
  ;;
esac
