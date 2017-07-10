#!/bin/sh
if [ ! -d "./cmake_gen" ]; then
  mkdir cmake_gen
fi

cd cmake_gen
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make -j 4

if [ $? -eq 0 ]; then
	cd ./tests
	./codex.tests
fi
