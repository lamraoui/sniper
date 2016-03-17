#!/bin/bash

NB_RUN=1
TIMEFORMAT='Time: %3U + %3S';

# Line-level
function runFFTFl {
    echo ""
    echo "FFTF (line-lvl) < $1 >"
    clang-mp-3.3 $1 -S -emit-llvm -O0 -gline-tables-only -o $1.bc >error 2>&1
    time ./../../src/sniper -ce -line -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden-outputs.txt $1.bc
    rm $1.bc
}
# Block-level
function runFFTFb {
    echo ""
    echo "FFTF (block-lvl) < $1 >"
    clang-mp-3.3 $1 -S -emit-llvm -O0 -gline-tables-only -o $1.bc >error 2>&1
    time ./../../src/sniper -ce -block -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden-outputs.txt $1.bc
    rm $1.bc
}
# Block-level
function runFFTFi {
    echo ""
    echo "FFTF (inst-lvl) < $1 >"
    clang-mp-3.3 $1 -S -emit-llvm -O0 -gline-tables-only -o $1.bc >error 2>&1
    time ./../../src/sniper -ce -inst -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden-outputs.txt $1.bc
    rm $1.bc
}



# Run all TCAS programs for both type of formula
echo "Running all TCAS versions with time()..."


echo "== FFTF (line, block, inst) ===================================="
for i in $(eval echo {1..41})
do
    echo "======================================"
    runFFTFb v$i/tcas.c
    echo ' '
    runFFTFl v$i/tcas.c
    echo ' '
    runFFTFi v$i/tcas.c
done
echo ' '

