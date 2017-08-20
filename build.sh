#!/bin/sh
if [ ! -d "./cmake_gen" ]; then
  mkdir cmake_gen
fi

cd cmake_gen
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_INCLUDE_HEADERS_IN_COMPILE_COMMANDS=ON ..
make -j 4

#if make success
if [ $? -eq 0 ]; then
  #if parameter exist
  if [ $# -eq 1 ]; then
    #if paramter is "run_test"
    if [ $1 = "run_tests" ]; then
      cd ./tests
      ./codex.tests
    fi
  fi
fi
