/*
   pre:  x is integer
   post: x == 0  implies y == 0
     and x > 0   implies y > 0
     and x < 0   implies y < 0
*/

/*
	Integer Overflow or Wraparound 
*/

#include <stdio.h>
#include <assert.h>

int foo(int);

int main(){
	int x;
	while(1){
		scanf("%d", &x);
		int y = foo(x);
		printf("%d\n", y);
	}
	return 0;
}

/* function foo doubles the input parameter x */
int foo(int x) {
	/* if x == 11 * 10000 * 10000 (assume int type is 32 bit), then the assertion will fail */
    int y = x + x;
    
    int judge = 0;
    judge += (x == 0) && (y == 0);
    judge += (x > 0) && (y > 0);
    judge += (x < 0) && (y < 0);
    
	assert(judge == 1);
	return y;
}







