#!/bin/bash

NB_RUN=100

# Partial Trace Formula - Small Granularity
function runPFTFsg {
    echo ""
    echo "< $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    total=0
    for i in $(eval echo {1..$NB_RUN})
    do
        output=$(./../../sniper -ce -ptf -function foo -cfile $1 $1.bc -small-gra)
        let "total += output"
    done
    let "total = total / NB_RUN"
    echo "total = $total"
	rm $1.bc	
}
# Partial Trace Formula - Big Granularity
function runPFTFbg {
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
# Classic Trace Formula - Small Granularity
function runTFsg {
    echo ""
    echo "< $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    total=0
    for i in $(eval echo {1..$NB_RUN})
    do
        output=$(./../../sniper -ce -function foo -cfile $1 $1.bc -small-gra)
        let "total += output"
    done
    let "total = total / NB_RUN"
    echo "total = $total"
    rm $1.bc
}
# Classic Trace Formula - Big Granularity
function runTFbg {
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
echo "=== PFTF SG ==================================="
#runPFTFsg maxmin/Maxmin6varKO3.c
runPFTFsg maxmin/Maxmin6varKO3-onebug.c
echo ""
echo ""

echo "=== PFTF BG ==================================="
#runPFTFbg maxmin/Maxmin6varKO3.c
runPFTFbg maxmin/Maxmin6varKO3-onebug.c
echo ""
