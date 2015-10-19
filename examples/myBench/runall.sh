#!/bin/bash

TIMEFORMAT='%3U %3S';

# Partial Trace Formula - Small Granularity
function runPFTFsg {
	echo ""
	echo "< $1 >"
	clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
	time ./../../sniper -ce -ptf -function foo -cfile $1 $1.bc -small-gra
	rm $1.bc	
}
# Partial Trace Formula - Big Granularity
function runPFTFbg {
	echo ""
	echo "< $1 >"
	clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
	time ./../../sniper -check-cfg-model -ce -ptf -function foo -cfile $1 $1.bc
	rm $1.bc	
}
# Classic Trace Formula - Small Granularity
function runTFsg {
	echo ""
	echo "< $1 >"
	clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
	time ./../../sniper -ce -function foo -cfile $1 $1.bc -small-gra
	rm $1.bc	
}
# Classic Trace Formula - Big Granularity
function runTFbg {
	echo ""
	echo "< $1 >"
	clang $1 -S -emit-llvm -O0 -g -o $1.bc >error 2>&1
	time ./../../sniper -ce -function foo -cfile $1 $1.bc 
	rm $1.bc	
}

# Run all program for all type of formula 
# and both type of granularity 
echo "=== PFTF BG ==================================="
echo ""
runPFTFbg MinmaxKO.c
runPFTFbg absminus/AbsMinusKO.c
runPFTFbg absminus/AbsMinusKO2.c
runPFTFbg absminus/AbsMinusKO3.c
runPFTFbg tritype/TritypeKO.c
runPFTFbg tritype/TritypeKO2.c
runPFTFbg tritype/TritypeKO2V2.c
runPFTFbg tritype/TritypeKO3.c
runPFTFbg tritype/TritypeKO4.c
runPFTFbg tritype/TritypeKO5.c
runPFTFbg triperim/TriPerimetreKO.c
runPFTFbg triperim/TriPerimetreKOV2.c
runPFTFbg triperim/TriPerimetreKO2.c
runPFTFbg triperim/TriPerimetreKO3.c
runPFTFbg maxmin/Maxmin6varKO.c
runPFTFbg maxmin/Maxmin6varKO2.c
runPFTFbg maxmin/Maxmin6varKO3.c
runPFTFbg maxmin/Maxmin6varKO4.c
echo ""
echo ""
echo "=== TF BG ==================================="
echo ""
runTFbg MinmaxKO.c
runTFbg absminus/AbsMinusKO.c
runTFbg absminus/AbsMinusKO2.c
runTFbg absminus/AbsMinusKO3.c
runTFbg tritype/TritypeKO.c
runTFbg tritype/TritypeKO2.c
runTFbg tritype/TritypeKO2V2.c
runTFbg tritype/TritypeKO3.c
runTFbg tritype/TritypeKO4.c
runTFbg tritype/TritypeKO5.c
runTFbg triperim/TriPerimetreKO.c
runTFbg triperim/TriPerimetreKOV2.c
runTFbg triperim/TriPerimetreKO2.c
runTFbg triperim/TriPerimetreKO3.c
runTFbg maxmin/Maxmin6varKO.c
runTFbg maxmin/Maxmin6varKO2.c
runTFbg maxmin/Maxmin6varKO3.c
runTFbg maxmin/Maxmin6varKO4.c
echo ""

echo "=== PFTF SG ==================================="
echo ""
runPFTFsg MinmaxKO.c
runPFTFsg absminus/AbsMinusKO.c
runPFTFsg absminus/AbsMinusKO2.c
runPFTFsg absminus/AbsMinusKO3.c
runPFTFsg tritype/TritypeKO.c
runPFTFsg tritype/TritypeKO2.c
runPFTFsg tritype/TritypeKO2V2.c
runPFTFsg tritype/TritypeKO3.c
runPFTFsg tritype/TritypeKO4.c
runPFTFsg tritype/TritypeKO5.c
runPFTFsg triperim/TriPerimetreKO.c
runPFTFsg triperim/TriPerimetreKOV2.c
runPFTFsg triperim/TriPerimetreKO2.c
runPFTFsg triperim/TriPerimetreKO3.c
runPFTFsg maxmin/Maxmin6varKO.c
runPFTFsg maxmin/Maxmin6varKO2.c
runPFTFsg maxmin/Maxmin6varKO3.c
runPFTFsg maxmin/Maxmin6varKO4.c
echo ""
echo ""

echo "=== TF SG ==================================="
echo ""
runTFsg MinmaxKO.c
runTFsg absminus/AbsMinusKO.c
runTFsg absminus/AbsMinusKO2.c
runTFsg absminus/AbsMinusKO3.c
runTFsg tritype/TritypeKO.c
runTFsg tritype/TritypeKO2.c
runTFsg tritype/TritypeKO2V2.c
runTFsg tritype/TritypeKO3.c
runTFsg tritype/TritypeKO4.c
runTFsg tritype/TritypeKO5.c
runTFsg triperim/TriPerimetreKO.c
runTFsg triperim/TriPerimetreKOV2.c
runTFsg triperim/TriPerimetreKO2.c
runTFsg triperim/TriPerimetreKO3.c
runTFsg maxmin/Maxmin6varKO.c
runTFsg maxmin/Maxmin6varKO2.c
runTFsg maxmin/Maxmin6varKO3.c
runTFsg maxmin/Maxmin6varKO4.c
echo ""
echo ""