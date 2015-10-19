#!/bin/bash

TIMEFORMAT='%3U %3S';

# Hardened Trace Formula - Small Granularity
function runHFTFsg {
	echo ""
	echo "< $1 >"
	clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
	time ./../../sniper -ce -htf -function foo -cfile $1 $1.bc -small-gra
	rm $1.bc	
}
# Hardened Trace Formula - Big Granularity
function runHFTFbg {
	echo ""
	echo "< $1 >"
	clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
	time ./../../sniper -v -ce -htf -function foo -cfile $1 $1.bc
	rm $1.bc	
}
# Classic Trace Formula - Small Granularity
function runFFTFsg {
	echo ""
	echo "< $1 >"
	clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
	time ./../../sniper -ce -function foo -cfile $1 $1.bc -small-gra
	rm $1.bc	
}
# Classic Trace Formula - Big Granularity
function runFFTFbg {
	echo ""
	echo "< $1 >"
	clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
	time ./../../sniper -v -ce -function foo -cfile $1 $1.bc
	rm $1.bc	
}

# Run all program for all type of formula 
# and both type of granularity 
echo "=== HFTF BG ==================================="
echo ""
runHFTFbg MinmaxKO.c
runHFTFbg absminus/AbsMinusKO.c
runHFTFbg absminus/AbsMinusKO2.c
runHFTFbg absminus/AbsMinusKO3.c
runHFTFbg tritype/TritypeKO.c
runHFTFbg tritype/TritypeKO2.c
runHFTFbg tritype/TritypeKO2V2.c
runHFTFbg tritype/TritypeKO3.c
runHFTFbg tritype/TritypeKO4.c
runHFTFbg tritype/TritypeKO5.c
runHFTFbg triperim/TriPerimetreKO.c
runHFTFbg triperim/TriPerimetreKOV2.c
runHFTFbg triperim/TriPerimetreKO2.c
runHFTFbg triperim/TriPerimetreKO3.c
runHFTFbg maxmin/Maxmin6varKO.c
runHFTFbg maxmin/Maxmin6varKO2.c
runHFTFbg maxmin/Maxmin6varKO3.c
runHFTFbg maxmin/Maxmin6varKO4.c
echo ""
echo ""
echo "=== FFTF BG ==================================="
echo ""
runFFTFbg MinmaxKO.c
runFFTFbg absminus/AbsMinusKO.c
runFFTFbg absminus/AbsMinusKO2.c
runFFTFbg absminus/AbsMinusKO3.c
runFFTFbg tritype/TritypeKO.c
runFFTFbg tritype/TritypeKO2.c
runFFTFbg tritype/TritypeKO2V2.c
runFFTFbg tritype/TritypeKO3.c
runFFTFbg tritype/TritypeKO4.c
runFFTFbg tritype/TritypeKO5.c
runFFTFbg triperim/TriPerimetreKO.c
runFFTFbg triperim/TriPerimetreKOV2.c
runFFTFbg triperim/TriPerimetreKO2.c
runFFTFbg triperim/TriPerimetreKO3.c
runFFTFbg maxmin/Maxmin6varKO.c
runFFTFbg maxmin/Maxmin6varKO2.c
runFFTFbg maxmin/Maxmin6varKO3.c
runFFTFbg maxmin/Maxmin6varKO4.c
echo ""

echo "=== HFTF SG ==================================="
echo ""
runHFTFsg MinmaxKO.c
runHFTFsg absminus/AbsMinusKO.c
runHFTFsg absminus/AbsMinusKO2.c
runHFTFsg absminus/AbsMinusKO3.c
runHFTFsg tritype/TritypeKO.c
runHFTFsg tritype/TritypeKO2.c
runHFTFsg tritype/TritypeKO2V2.c
runHFTFsg tritype/TritypeKO3.c
runHFTFsg tritype/TritypeKO4.c
runHFTFsg tritype/TritypeKO5.c
runHFTFsg triperim/TriPerimetreKO.c
runHFTFsg triperim/TriPerimetreKOV2.c
runHFTFsg triperim/TriPerimetreKO2.c
runHFTFsg triperim/TriPerimetreKO3.c
runHFTFsg maxmin/Maxmin6varKO.c
runHFTFsg maxmin/Maxmin6varKO2.c
runHFTFsg maxmin/Maxmin6varKO3.c
runHFTFsg maxmin/Maxmin6varKO4.c
echo ""
echo ""

echo "=== FFTF SG ==================================="
echo ""
runFFTFsg MinmaxKO.c
runFFTFsg absminus/AbsMinusKO.c
runFFTFsg absminus/AbsMinusKO2.c
runFFTFsg absminus/AbsMinusKO3.c
runFFTFsg tritype/TritypeKO.c
runFFTFsg tritype/TritypeKO2.c
runFFTFsg tritype/TritypeKO2V2.c
runFFTFsg tritype/TritypeKO3.c
runFFTFsg tritype/TritypeKO4.c
runFFTFsg tritype/TritypeKO5.c
runFFTFsg triperim/TriPerimetreKO.c
runFFTFsg triperim/TriPerimetreKOV2.c
runFFTFsg triperim/TriPerimetreKO2.c
runFFTFsg triperim/TriPerimetreKO3.c
runFFTFsg maxmin/Maxmin6varKO.c
runFFTFsg maxmin/Maxmin6varKO2.c
runFFTFsg maxmin/Maxmin6varKO3.c
runFFTFsg maxmin/Maxmin6varKO4.c
echo ""
echo ""