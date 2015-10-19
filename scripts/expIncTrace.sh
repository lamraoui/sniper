#!/bin/bash
if [ $# -lt 3 ]
then
    echo "error: parameters"
    exit 0
fi
clang $1 -S -emit-llvm -O0 -g -o $1.bc  #>error 2>&1
for i in $(eval echo {1..$3})
do
    if [ $2 == "fail" ]
    then
        ./sniper -ce -ptf -print-dura -function foo -cfile $1 $1.bc -max-fail $i -max-succ 1
    elif [ $2 == "succ" ]
    then
        ./sniper -ce -ptf -print-dura -function foo -cfile $1 $1.bc -max-succ $i
    else
        echo "error: second param should be 'fail' of 'succ'"
        exit 0
    fi
done
rm $1.bc

# Maxmin6varKO3: fail 56, succ 404
# Maxmin6varKO4: fail 61, succ 403