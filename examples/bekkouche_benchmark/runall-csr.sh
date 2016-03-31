#!/bin/bash

# runall-time.sh
#
# ----------------------------------------------------------------------
#
# Experiment for SNIPER on the Bekkouche Benchmark
#
# This experiment aims at comparing the code size reduction (CSR) 
# obtained with SNIPER with different trace formula encodings.
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


# Granularity level: -inst, -line, -block.
GRANU_LEVEL=-line


# Run SNIPER
function run {
    if [ "$3" == "HFTF" ]; then
        ENCODING=-htf
    fi
    echo ""
    echo "< $1 >"
    clang-mp-3.3 $1 -S -emit-llvm -O0 -gline-tables-only -o $1.bc >error 2>&1
    ./../../src/sniper -mhs -loc=$2 -ce $GRANU_LEVEL $ENCODING -function foo -cfile $1 $1.bc
    rm $1.bc
}

# Run SNIPER with Hardened Flow-sensitive Trace Formula (HFTF)
function runHFTF {
    run $1 $2 "HFTF"
}

# Run SNIPER with Full Flow-sensitive Trace Formula (FFTF)
function runFFTF {
    run $1 $2 "FFTF" 
}

# Check if SNIPER binarie exists
if [ ! -f ../../src/sniper ]; then
    echo "SNIPER binarie not found!"
    exit 1
fi

# Run all programs for both types of formulas
echo "Experiment for SNIPER on the Bekkouche Benchmark."
echo ""
echo "Granularity level:" $GRANU_LEVEL
echo "Running" $NB_RUN "times each program..."
echo ""

echo "=== HFTF ==================================="
runHFTF MinmaxKO.c 33
runHFTF absminus/AbsMinusKO.c 32
runHFTF absminus/AbsMinusKO2.c 31
runHFTF absminus/AbsMinusKO3.c 29
runHFTF tritype/TritypeKO.c 77
runHFTF tritype/TritypeKO2.c 78
runHFTF tritype/TritypeKO2V2.c 77
runHFTF tritype/TritypeKO3.c 77
runHFTF tritype/TritypeKO4.c 78
runHFTF tritype/TritypeKO5.c 77
runHFTF triperim/TriPerimetreKO.c 82
runHFTF triperim/TriPerimetreKOV2.c 84
runHFTF triperim/TriPerimetreKO2.c 83
runHFTF triperim/TriPerimetreKO3.c 80
runHFTF maxmin/Maxmin6varKO.c 176
runHFTF maxmin/Maxmin6varKO2.c 178
runHFTF maxmin/Maxmin6varKO3.c 176
runHFTF maxmin/Maxmin6varKO4.c 175
echo ""
echo ""

echo "=== FFTF ==================================="
runFFTF MinmaxKO.c 33
runFFTF absminus/AbsMinusKO.c 32
runFFTF absminus/AbsMinusKO2.c 31
runFFTF absminus/AbsMinusKO3.c 29
runFFTF tritype/TritypeKO.c 77
runFFTF tritype/TritypeKO2.c 78
runFFTF tritype/TritypeKO2V2.c 77
runFFTF tritype/TritypeKO3.c 77
runFFTF tritype/TritypeKO4.c 78
runFFTF tritype/TritypeKO5.c 77
runFFTF triperim/TriPerimetreKO.c 82
runFFTF triperim/TriPerimetreKOV2.c 84
runFFTF triperim/TriPerimetreKO2.c 83
runFFTF triperim/TriPerimetreKO3.c 80
runFFTF maxmin/Maxmin6varKO.c 176
runFFTF maxmin/Maxmin6varKO2.c 178
runFFTF maxmin/Maxmin6varKO3.c 176
runFFTF maxmin/Maxmin6varKO4.c 175
echo ""
echo ""

