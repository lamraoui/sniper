/*
   pre:  n is a positive integer
   post: function foo calculates the approximate value of the square root of
		n, and the result is presented by fraction
*/
#include <assert.h>

//#define MAX 20000
#define MAX 10

// The algorithm is shown in Example 1.1.1 with sound theoretical foundation.

int gcd(int,int);
int mod(int,int);
/*
#include <stdio.h>
#include <assert.h>
void foo(int);

int main(){
	int i;
	for(i = 3;i <= 20;i++)
		foo(i);
	return 0;
}
*/
void foo(int n){
	// This function calculates the approximate value of square root of n.   
	// Please notice that n should be a small integer (maybe numbers less than 100 will be good choice). 
	//assume(n >= 3 && n <= 20);
	
	// find a proper initial value
	int num = 1;// numerator
	int den = 1;// denominator
	while(num * num < den * den * n)
		num++;
	
	// iteration part
	if(num * num != den * den * n){
		while(num < MAX){// to prevent integer overflow
			int a = num * num + n * den * den;		
			int b = 2 * num * den;
			num = a;
			den = b;
			int d = gcd(a,b);
			num /= d;
			den /= d;			
		}
	}		
	assert(num > den);	
}

int gcd(int a,int b){
	// This function calculates the greatest common divisor of a and b.
	// requirement of input: a > 0, b >= 0
	if(b == 0)
		return a;
	return gcd(b,mod(a,b));
}

int mod(int a,int b){
	// This function calculates a % b.
	// requirement of input: a >= 0, b > 0
	while(a >= b)
		a -= b;
	return a;
}
