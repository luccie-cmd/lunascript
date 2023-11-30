#!/bin/bash

mkdir -p build
rm -f build/luna
cmake -B build
cmake --build build
cd build
if [ "$1" == "install" ]; then
    cd build
    sudo make install
    cd ..
fi