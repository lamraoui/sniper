#!/bin/bash




echo ' '
echo '============== FFTF vs PFTF ===================='
echo ' '

for i in $(eval echo {$1..$2})
do
echo 'v'$i
./run foo ./examples/tcas_nomain_ret/v$i/tcas.c 5
echo ' '
./run foo ./examples/tcas_nomain_ret/v$i/tcas.c 6
echo ' '
echo ' '
done








echo ' '
echo '============== FFTF ===================='
echo ' '

for i in $(eval echo {$1..$2})
do
echo '<tcas v'$i'>'
./run foo ./examples/tcas_nomain_ret/v$i/tcas.c 5
echo ' '
done




echo ' '
echo '=============== PFTF ==================='
echo ' '

for i in $(eval echo {$1..$2})
do
echo '<tcas v'$i'>'
./run foo ./examples/tcas_nomain_ret/v$i/tcas.c 6
echo ' '
done

