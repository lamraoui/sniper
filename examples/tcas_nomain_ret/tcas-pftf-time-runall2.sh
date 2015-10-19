#!/bin/bash

NB_RUN=1
TIMEFORMAT='%3U + %3S';

# Partial Folw-sensitive Trace Formula
function runPFTF {
    echo ""
    echo "PFTF < $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    time ./../../sniper -ce -ptf -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden_outputs.txt $1.bc
    rm $1.bc
}
# Full Flow-sensitive Trace Formula
function runFFTF {
    echo ""
    echo "FFTF < $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    time ./../../sniper -ce -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden_outputs.txt $1.bc
    rm $1.bc
}




# Run all TCAS programs for both type of formula
echo "Running all TCAS versions with time()..."


## TORUN:
#runFFTF v36/tcas.c


echo "=== PFTF ==================================="
for i in $(eval echo {1..41})
do
    runPFTF v$i/tcas.c
done
echo ' '
echo ' '

echo "=== FFTF ==================================="
for i in $(eval echo {1..41})
do
    runFFTF v$i/tcas.c
done
echo ' '


#echo "== PFTF vs. FFTF ===================================="
#for i in $(eval echo {1..41})
#do
#    echo "======================================"
#	runPFTF v$i/tcas.c
#    echo ' '
#    echo ' '
#    runFFTF v$i/tcas.c
#done
#echo ' '

