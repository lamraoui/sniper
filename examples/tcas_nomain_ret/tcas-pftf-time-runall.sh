#!/bin/bash

NB_RUN=1

# Partial Folw-sensitive Trace Formula
function runPFTF {
    echo ""
    echo "< $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    total=0
    for i in $(eval echo {1..$NB_RUN})
    do
        output=$(./../../sniper -ce -ptf -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden_outputs.txt $1.bc)
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
        output=$(./../../sniper -ce -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden_outputs.txt $1.bc)
        let "total += output"
    done
    let "total = total / NB_RUN"
    echo "total = $total"
    rm $1.bc
}




# Run all TCAS programs for both type of formula
echo "Running" $NB_RUN "times all TCAS versions..."


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



echo "=== PFTF (good) ==================================="
#For the versions no. 2, 4 6-9,13-25, 29-32, and 36-41
runPFTF v2/tcas.c
runPFTF v4/tcas.c
runPFTF v6/tcas.c
runPFTF v7/tcas.c
runPFTF v8/tcas.c
runPFTF v9/tcas.c
for i in $(eval echo {13..25})
do
runPFTF v$i/tcas.c
done
for i in $(eval echo {29..32})
do
runPFTF v$i/tcas.c
done
for i in $(eval echo {36..41})
do
runPFTF v$i/tcas.c
done

