/*
   pre:  16 > x >= 0
   post: ((0 <= x <= 1)  implies (y == x + 6))
     and ((1 < x <= 6)   implies (y == 3 - x))
     and ((6 < x < 10)   implies (y == 6))
     and ((10 <= x < 16) implies (y == -1))
*/

/*
	but, in fact, when x is 1, y will be -3 instead of the expected value 7
*/

#include <stdio.h>
#include <assert.h>

int y;

int foo(int);

int main(){
	int x;
	for(x = 0;x < 16;x++){
		printf("x is:%d, y is:%d\n",x,foo(x));
	}
	return 0;
}

int foo (int x) {
	int a, b, u, v;
	
	if (x < 10) {
		a = 4; b = 1;
	} else {
		a = 1; b = 4;
	}
	
	if (x + a > 10) {
		u = a + b; v = b;
	} else if (x + a <= 5) {
		u = a + b; v = x + b;
	} else {
		u = x + b; v = a;
	}
	
	if (v < 2) {
		y = u + v;
	} else {
		y = v - u;
	}
	
	int judge = 0;
	judge += (x < 0) || (x >= 16);
	judge += (0 <= x && x <= 1) && (y == x + 6);
	judge += (1 < x && x<= 6) && (y == 3 - x);
	judge += (6 < x && x < 10) && (y == 6);
	judge += (10 <= x && x < 16) && (y == -1);
	assert(judge == 1);
	
	return(y);   
}
