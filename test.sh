#!/usr/bin/env bash
# This runs all the files in the test folder with extension .luna and drops their results in [NAME].luna.txt

# set -e

./build.sh

test_dir="./test"

# Compile all .luna files in the source directory and output to the destination directory
for file in $test_dir/*.luna
do
    if [ -f "$file" ]; then
        echo "Compiling $file"
        if [ "$file" == "./test/err.luna" ]; then
            ./build/luna "$file" > "$file.txt" --print-ast --no-errors
        else
            ./build/luna "$file" > "$file.txt" --print-ast
        fi
    fi
done
