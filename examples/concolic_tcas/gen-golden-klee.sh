#!/bin/sh

for i in {1..41} 
do 
	cat ./v$i/inputs_dfs_klee.txt | xargs -L 1 ./tcas > ./v$i/golden_dfs_klee.txt	
done
