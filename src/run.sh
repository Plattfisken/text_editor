#!/bin/bash
PROGRAM_NAME="$(./build.sh)";
ARGS="../src/test_file.txt"
if [ -n "$PROGRAM_NAME" ]; then
    ../build/"$PROGRAM_NAME" $ARGS;
fi
