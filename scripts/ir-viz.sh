#!/bin/bash

opt -mem2reg -dot-cfg file.bc
# Tps, Tpdf, Teps...
dot -Tpng file.dot -o outfile.png