#!/bin/bash

# runall-time.sh
#
# ----------------------------------------------------------------------
#
# Experiment for SNIPER on the Bekkouche Benchmark
#
# This experiment aims at comparing the results of
# SNIPER with different trace formula encodings.
#
# ----------------------------------------------------------------------
#                SNIPER : Automatic Fault Localization 
#
# Copyright (C) 2016 Si-Mohamed LAMRAOUI
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program (see LICENSE.TXT).  
# If not, see <http://www.gnu.org/licenses/>.
# ----------------------------------------------------------------------
#
# \author Si-Mohamed LAMRAOUI
# \date   18 March 2016


# Number of time each program is analysed to
# have an average computing time.
NB_RUN=100

# Output time format.
TIMEFORMAT='%3U';

# Granularity level: -inst, -line, -block.
GRANU_LEVEL=-line


# Run SNIPER
function run {
    if [ "$2" == "HFTF" ]; then
        ENCODING=-htf
    fi
    echo ""
    echo "< $1 >"
    clang-mp-3.3 $1 -S -emit-llvm -O0 -gline-tables-only -o $1.bc >error 2>&1
    total=0
    for i in $(eval echo {1..$NB_RUN})
    do
        output=$({ time ./../../src/sniper -ce $GRANU_LEVEL $ENCODING -function foo -cfile $1 $1.bc;} 2>&1)
        total=$(echo $output + $total | bc -l)
    done
    total=$(echo "scale=4; $total / $NB_RUN" | bc -l)
    echo "total = $total"
    rm $1.bc
}

# Run SNIPER with Hardened Flow-sensitive Trace Formula (HFTF)
function runHFTF {
    run $1 "HFTF"
}

# Run SNIPER with Full Flow-sensitive Trace Formula (FFTF)
function runFFTF {
    run $1 "FFTF"
}


# Run all programs for both types of formulas
echo "Running" $NB_RUN "times each program..."
echo ""

echo "=== HFTF ==================================="
runHFTF MinmaxKO.c
runHFTF absminus/AbsMinusKO.c
runHFTF absminus/AbsMinusKO2.c
runHFTF absminus/AbsMinusKO3.c
runHFTF tritype/TritypeKO.c
runHFTF tritype/TritypeKO2.c
runHFTF tritype/TritypeKO2V2.c
runHFTF tritype/TritypeKO3.c
runHFTF tritype/TritypeKO4.c
runHFTF tritype/TritypeKO5.c
runHFTF triperim/TriPerimetreKO.c
runHFTF triperim/TriPerimetreKOV2.c
runHFTF triperim/TriPerimetreKO2.c
runHFTF triperim/TriPerimetreKO3.c
runHFTF maxmin/Maxmin6varKO.c
runHFTF maxmin/Maxmin6varKO2.c
runHFTF maxmin/Maxmin6varKO3.c
runHFTF maxmin/Maxmin6varKO4.c
echo ""
echo ""

echo "=== FFTF ==================================="
runFFTF MinmaxKO.c
runFFTF absminus/AbsMinusKO.c
runFFTF absminus/AbsMinusKO2.c
runFFTF absminus/AbsMinusKO3.c
runFFTF tritype/TritypeKO.c
runFFTF tritype/TritypeKO2.c
runFFTF tritype/TritypeKO2V2.c
runFFTF tritype/TritypeKO3.c
runFFTF tritype/TritypeKO4.c
runFFTF tritype/TritypeKO5.c
runFFTF triperim/TriPerimetreKO.c
runFFTF triperim/TriPerimetreKOV2.c
runFFTF triperim/TriPerimetreKO2.c
runFFTF triperim/TriPerimetreKO3.c
runFFTF maxmin/Maxmin6varKO.c
runFFTF maxmin/Maxmin6varKO2.c
runFFTF maxmin/Maxmin6varKO3.c
runFFTF maxmin/Maxmin6varKO4.c
echo ""
echo ""