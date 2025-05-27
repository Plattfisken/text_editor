#!/bin/bash
PROGRAM_NAME="$(./build.sh)";
ARGS="../src/main.c"
if [ -n "$PROGRAM_NAME" ]; then
    ../build/"$PROGRAM_NAME" $ARGS;
fi
