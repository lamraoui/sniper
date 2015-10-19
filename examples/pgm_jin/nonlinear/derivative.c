/*
   pre:  
   post: The function foo will calculate the value of derivative function at x
   		since f(x) = x * x, so in fact, the derivative function of f(x) will be f'(x) = 2 * x
*/

// The algorithm is shown in "1.1.4 Finite Difference Approximations"

int num;// this variable stores the numerator value of the result
int den;// this variable stores the denominator value of the result

int gcd(int,int);
int mod(int,int);
void minus(int,int,int,int);
/*
#include <stdio.h>
#include <assert.h>
void foo(int,int,int);

int main(){
	foo(8,1,10);
	return 0;
}
*/
void foo(int x,int a,int b){
	// foo calculates the value of derivative function at x
	// let h = a / b 
	// then the value can be approximated by f(x + h) - f(x - h)) / (2 * h)
	
	//assume(x == 8);
	//assume(a == 1);
	//assume(b == 10);
	
    int num_of_former = (b * x - a) * (b * x - a);
    int den_of_former = b * b;
    
    int num_of_latter = (b * x + a) * (b * x + a);
    int den_of_latter = b * b;
    
    minus(num_of_latter,den_of_latter,num_of_former,den_of_former);
    num *= b;
    den *= 2 * a;
    int d = gcd(num,den);
    num /= d;
    den /= d;
    
    assert(num == den * 2 * x);
}

void minus(int num1,int den1,int num2,int den2){
	/* 
		This function calculate the result of num1 / den1 - num2 / den2,
		the result is stored in 
		the variable "num" and "den"
	*/
	num = num1 * den2 - num2 * den1;
	den = den1 * den2;
	int d = gcd(num,den);// d is the greatest common divisor of num and den
	num /= d;
	den /= d;
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
