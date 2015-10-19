#!/bin/bash

NB_RUN=1

# Partial Flow-sensitive Trace Formula
function runPFTF {
    echo ""
    echo "< $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    total=0
    for i in $(eval echo {1..$NB_RUN})
    do
        output=$(./../../sniper -ce -ptf -function foo -cfile $1 -ts testcases-argok-nooverflow.txt -go golden_outputs_argok_nooverflow.txt $1.bc)
        let "total += output"
    done
    let "total = total / NB_RUN"
    echo "total = $total"
    rm $1.bc
}
# Full Flow-sensitive Trace Formula
function runFFTF {
    echo ""
    echo "< $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    total=0
    for i in $(eval echo {1..$NB_RUN})
    do
        output=$(./../../sniper -ce -function foo -cfile $1 -ts testcases-argok-nooverflow.txt -go golden_outputs_argok_nooverflow.txt $1.bc)
        let "total += output"
    done
    let "total = total / NB_RUN"
    echo "total = $total"
    rm $1.bc
}



# Run all TCAS programs for both type of formula
echo "Runing " $NB_RUN "times all TCAS versions (no overflow)..."  
echo "=== PFTF ==================================="
for i in $(eval echo {1..41})
do
	runPFTF v$i/tcas.c
done
echo ' '
echo ' '

echo '============== FFTF ===================='
for i in $(eval echo {1..41})
do
	runFFTF v$i/tcas.c
done
echo ' '

