#!/bin/bash
PROGRAM_NAME="$(./build.sh)";
ARGS="../src/proed_ideas.txt"
if [ -n "$PROGRAM_NAME" ]; then
    ../build/"$PROGRAM_NAME" $ARGS;
fi
