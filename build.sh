#!/bin/bash

mkdir -p build
rm -f build/luna
cmake -B build
cmake --build build
echo "Runnig main.luna"
./build/luna ./test/main.luna