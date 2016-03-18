#!/bin/bash

#====================================================
# Experiment for SNIPER on the TCAS Benchmark
# 
# This experiment aims at comparing the results of 
# SNIPER with FFTF with different granularity levels.
#
# @author  Si-Mohamed Lamraoui
# @date    18 March 2016
#====================================================

NB_RUN=1
TIMEFORMAT='Time: %3U + %3S';

function run {
    if [ "$2" == "inst" ]; then
        GRANU_LEVEL=-inst
    elif [ "$2" == "line" ]; then
        GRANU_LEVEL=-line
    else
        GRANU_LEVEL=-block
    fi
    clang-mp-3.3 $1 -S -emit-llvm -O0 -gline-tables-only -o $1.bc >error 2>&1
    time ./../../src/sniper -ce $GRANU_LEVEL -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden-outputs.txt $1.bc
    rm $1.bc
}

# Inst-level
function runFFTFi {
    echo ""
    echo "FFTF (inst-lvl) < $1 >"
     run $1 "inst"
}

# Line-level
function runFFTFl {
    echo ""
    echo "FFTF (line-lvl) < $1 >"
    run $1 "line"
}

# Block-level
function runFFTFb {
    echo ""
    echo "FFTF (block-lvl) < $1 >"
     run $1 "block"
}

# Run all TCAS programs for both type of formula
echo "Running all TCAS versions with time()..."
echo ""

echo "== FFTF (inst, line, block) ===================================="
for i in $(eval echo {1..41})
do
    runFFTFi v$i/tcas.c
    echo ' '
    runFFTFl v$i/tcas.c
    echo ' '
    runFFTFb v$i/tcas.c
    echo "======================================"
done
echo ' '

