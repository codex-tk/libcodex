#!/bin/sh
if [ ! -d "./cmake_gen" ]; then
  mkdir cmake_gen
fi

cd cmake_gen
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_INCLUDE_HEADERS_IN_COMPILE_COMMANDS=ON..
make -j 4

if [ $? -eq 0 ]; then
	cd ./tests
	./codex.tests
fi
