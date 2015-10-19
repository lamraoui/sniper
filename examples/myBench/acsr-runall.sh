#!/bin/bash

# Partial Trace Formula
function runPFTF {
    echo ""
    echo "PFTF < $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    output=$(./../../sniper -ce -ptf -v -function foo -loc $2 -cfile $1 $1.bc)
    echo "$output"
    rm $1.bc
}
# Classic Trace Formula
function runFFTF {
    echo ""
    echo "FFTF < $1 >"
    clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
    output=$(./../../sniper -ce -v -function foo -loc $2 -cfile $1 $1.bc)
    echo "$output"
    rm $1.bc
}

# Run all programs for all type of formula
# and both type of granularity
echo "=== FFTF vs PFTF ==================================="
runFFTF MinmaxKO.c 33
runPFTF MinmaxKO.c 33
runFFTF absminus/AbsMinusKO.c 32
runPFTF absminus/AbsMinusKO.c 32
runFFTF absminus/AbsMinusKO2.c 31
runPFTF absminus/AbsMinusKO2.c 31
runFFTF absminus/AbsMinusKO3.c 29
runPFTF absminus/AbsMinusKO3.c 29
runFFTF tritype/TritypeKO.c 77
runPFTF tritype/TritypeKO.c 77
runFFTF tritype/TritypeKO2.c 78
runPFTF tritype/TritypeKO2.c 78
runFFTF tritype/TritypeKO2V2.c 77
runPFTF tritype/TritypeKO2V2.c 77
runFFTF tritype/TritypeKO3.c 77
runPFTF tritype/TritypeKO3.c 77
runFFTF tritype/TritypeKO4.c 78
runPFTF tritype/TritypeKO4.c 78
runFFTF tritype/TritypeKO5.c 77
runPFTF tritype/TritypeKO5.c 77
runFFTF triperim/TriPerimetreKO.c 82
runPFTF triperim/TriPerimetreKO.c 82
runFFTF triperim/TriPerimetreKOV2.c 84
runPFTF triperim/TriPerimetreKOV2.c 84
runFFTF triperim/TriPerimetreKO2.c 83
runPFTF triperim/TriPerimetreKO2.c 83
runFFTF triperim/TriPerimetreKO3.c 80
runPFTF triperim/TriPerimetreKO3.c 80
runFFTF maxmin/Maxmin6varKO.c 176
runPFTF maxmin/Maxmin6varKO.c 176
runFFTF maxmin/Maxmin6varKO2.c 178
runPFTF maxmin/Maxmin6varKO2.c 178
runFFTF maxmin/Maxmin6varKO3.c 176
runPFTF maxmin/Maxmin6varKO3.c 176
runFFTF maxmin/Maxmin6varKO4.c 175
runPFTF maxmin/Maxmin6varKO4.c 175
echo ""
echo ""
