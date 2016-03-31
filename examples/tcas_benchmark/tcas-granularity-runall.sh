#!/bin/bash

# tcas-granularity-runall.sh
#
# ----------------------------------------------------------------------
#
# Experiment for SNIPER on the TCAS Benchmark
# 
# This experiment aims at comparing the results of 
# SNIPER with FFTF with different granularity levels.
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

NB_RUN=1
TIMEFORMAT='Time: %3U + %3S';

function run {
    if [ "$2" == "inst" ]; then
        GRANU_LEVEL=-inst
    elif [ "$2" == "line" ]; then
        GRANU_LEVEL=-line
    else
        GRANU_LEVEL=-block
    fi
    clang-mp-3.3 $1 -S -emit-llvm -O0 -gline-tables-only -o $1.bc >error 2>&1
    time ./../../src/sniper -ce $GRANU_LEVEL -function foo -cfile $1 -loc 173 -ts testcases-argok.txt -go golden-outputs.txt $1.bc
    rm $1.bc
}

# Inst-level
function runFFTFi {
    echo ""
    echo "FFTF (inst-lvl) < $1 >"
     run $1 "inst"
}

# Line-level
function runFFTFl {
    echo ""
    echo "FFTF (line-lvl) < $1 >"
    run $1 "line"
}

# Block-level
function runFFTFb {
    echo ""
    echo "FFTF (block-lvl) < $1 >"
     run $1 "block"
}

# Check if SNIPER binarie exists
if [ ! -f ../../src/sniper ]; then
    echo "SNIPER binarie not found!"
    exit 1
fi

# Run all TCAS programs for both type of formula
echo "Running all TCAS versions with time()..."
echo ""

echo "== FFTF (inst, line, block) ===================================="
for i in $(eval echo {1..41})
do
    runFFTFi v$i/tcas.c
    echo ' '
    runFFTFl v$i/tcas.c
    echo ' '
    runFFTFb v$i/tcas.c
    echo "======================================"
done
echo ' '

