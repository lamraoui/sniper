/*
   pre:  x is an integer
   post: the return value of function foo
		will show whether the input parameter x is positive
*/

#include <stdio.h>
#include <assert.h>

int foo(int x);

int main(){
	foo(0);
	return 0;
}

int foo(int x) {
    int y;/* uninitialized variable */
    if(x > 0)
    	y++;
    assert(y == 1);
    return y;
}
