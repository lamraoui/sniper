/*
The program AbsMinus (implemented in function foo) takes 
two integers i and j as input and returns the absolute value of i-j.

There is an error in this program that is in the assignment 
"k=k+2", which should be "k=k+2. By putting as input 
{i=0, j=1}, the error makes the condition  "(k == 1 && i != j)" 
false and therefore the execution of the instruction "result=i-j",
which causes an incrorrect value to be output, -1 instead of 1. 

@author: Mohammed Bekkouche
@Web:    http://www.i3s.unice.fr
*/

#include <sniper/sniper.h>

/* returns |i-j|, the absolute value of i minus j */
int foo (int i, int j) {//__CPROVER_assume((i==0) && (j==1));
    int result;
    int k = 0;
    if (i <= j) {
        k = k+2; // error in the assignment : k = k+2 instead of k = k+1
    }
    if (k == 1 && i != j) {
        result = j-i; 		
    }
    else {
        result = i-j;
    }
    //assert(result==1);
    sniper_assert( (i<j && result==j-i) || (i>=j && result==i-j));
}
