#!/bin/bash

# Compile original version of TCAS
gcc tcas.c -o tcas >err.txt 2>&1
# Generate golden outputs 
cat testcases-argok.txt | xargs -L 1 ./tcas > golden-ouputs.txt 

for i in {1..41}
do
	# Compile TCAS version i
	gcc ./v$i/tcas.c -o ./v$i/exe >err.txt 2>&1
	
	# Generate failing test cases
	echo "Generating failing test-cases for tcas v$i"
	rm -f ./v$i/failing-inputs.txt
	rm -f ./v$i/golden-outputs.txt
	nbFailed=0
	while read tc; do
		ret1=$(./tcas $tc)
		ret2=$(./v$i/exe $tc)
		if [ $ret1 -ne $ret2 ] 
		then
  			echo $tc >> ./v$i/failing-inputs.txt
  			echo $ret1 >> ./v$i/golden-outputs.txt
  			nbFailed=$(($nbFailed+1))
  		fi
	done < testcases-argok.txt
   	echo "  Nb failed: $nbFailed"
   	echo ""
done