#!/bin/bash

NB_RUN=1


# Partial Trace Formula
function runPFTF {
    echo ""
    echo "< $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    total=0
    for i in $(eval echo {1..$NB_RUN})
    do
        output=$(./../../sniper -ce -ptf -function foo -cfile $1 $1.bc)
        let "total += output"
    done
    let "total = total / NB_RUN"
    echo "total = $total"
    rm $1.bc
}
# Classic Trace Formula
function runFFTF {
    echo ""
    echo "< $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    total=0
    for i in $(eval echo {1..$NB_RUN})
    do
        output=$(./../../sniper -ce -function foo -cfile $1 $1.bc)
        let "total += output"
    done
    let "total = total / NB_RUN"
    echo "total = $total"
    rm $1.bc
}

# Run all program for all type of formula
# and both type of granularity
echo "Number of runs: " $NB_RUN
echo "=== PFTF ==================================="
runPFTF scfg/scfg2.c
runPFTF scfg/scfg10.c
runPFTF scfg/scfg100.c
runPFTF scfg/scfg200.c
runPFTF scfg/scfg300.c
runPFTF scfg/scfg400.c
runPFTF scfg/scfg500.c
runPFTF scfg/scfg600.c
runPFTF scfg/scfg700.c
runPFTF scfg/scfg800.c
runPFTF scfg/scfg900.c
runPFTF scfg/scfg1000.c
runPFTF scfg/scfg2000.c
runPFTF scfg/scfg3000.c
runPFTF scfg/scfg6000.c
echo ""
echo ""

echo "=== FFTF ==================================="
runFFTF scfg/scfg2.c
runFFTF scfg/scfg10.c
runFFTF scfg/scfg100.c
runFFTF scfg/scfg200.c
runFFTF scfg/scfg300.c
runFFTF scfg/scfg400.c
runFFTF scfg/scfg500.c
runFFTF scfg/scfg600.c
runFFTF scfg/scfg700.c
runFFTF scfg/scfg800.c
runFFTF scfg/scfg900.c
runFFTF scfg/scfg1000.c
runFFTF scfg/scfg2000.c
runFFTF scfg/scfg3000.c
runFFTF scfg/scfg6000.c
echo ""