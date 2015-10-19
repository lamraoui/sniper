#!/bin/bash

TIMEFORMAT='%3U %3S';

# Partial Trace Formula
function runHFTF {
    echo ""
    echo "HFTF < $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    time ./../../sniper -ce -htf -function foo -loc $2 -cfile $1 $1.bc
    rm $1.bc
}
# Classic Trace Formula
function runFFTF {
    echo ""
    echo "FFTF < $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    time ./../../sniper -ce -function foo -loc $2 -cfile $1 $1.bc
    rm $1.bc
}



echo "-------------------------------"
runFFTF maxmin/Maxmin6varKO3.c 176
runHFTF maxmin/Maxmin6varKO3.c 176
echo "-------------------------------"


# Run all programs for all type of formula
# and both type of granularity
echo "=== FFTF vs HFTF ==================================="
runFFTF MinmaxKO.c 33
runHFTF MinmaxKO.c 33
echo "-------------------------------"
runFFTF absminus/AbsMinusKO.c 32
runHFTF absminus/AbsMinusKO.c 32
echo "-------------------------------"
runFFTF absminus/AbsMinusKO2.c 31
runHFTF absminus/AbsMinusKO2.c 31
echo "-------------------------------"
runFFTF absminus/AbsMinusKO3.c 29
runHFTF absminus/AbsMinusKO3.c 29
echo "-------------------------------"
runFFTF tritype/TritypeKO.c 77
runHFTF tritype/TritypeKO.c 77
echo "-------------------------------"
runFFTF tritype/TritypeKO2.c 78
runHFTF tritype/TritypeKO2.c 78
echo "-------------------------------"
runFFTF tritype/TritypeKO2V2.c 77
runHFTF tritype/TritypeKO2V2.c 77
echo "-------------------------------"
runFFTF tritype/TritypeKO3.c 77
runHFTF tritype/TritypeKO3.c 77
echo "-------------------------------"
runFFTF tritype/TritypeKO4.c 78
runHFTF tritype/TritypeKO4.c 78
echo "-------------------------------"
runFFTF tritype/TritypeKO5.c 77
runHFTF tritype/TritypeKO5.c 77
echo "-------------------------------"
runFFTF triperim/TriPerimetreKO.c 82
runHFTF triperim/TriPerimetreKO.c 82
echo "-------------------------------"
runFFTF triperim/TriPerimetreKOV2.c 84
runHFTF triperim/TriPerimetreKOV2.c 84
echo "-------------------------------"
runFFTF triperim/TriPerimetreKO2.c 83
runHFTF triperim/TriPerimetreKO2.c 83
echo "-------------------------------"
runFFTF triperim/TriPerimetreKO3.c 80
runHFTF triperim/TriPerimetreKO3.c 80
echo "-------------------------------"
runFFTF maxmin/Maxmin6varKO.c 176
runHFTF maxmin/Maxmin6varKO.c 176
echo "-------------------------------"
runFFTF maxmin/Maxmin6varKO2.c 178
runHFTF maxmin/Maxmin6varKO2.c 178
echo "-------------------------------"
runFFTF maxmin/Maxmin6varKO3.c 176
runHFTF maxmin/Maxmin6varKO3.c 176
echo "-------------------------------"
runFFTF maxmin/Maxmin6varKO4.c 175
runHFTF maxmin/Maxmin6varKO4.c 175
echo ""
echo ""
