/*
   pre:  
   post: The function foo will calculate the area of a specified region.
   		The specified region: 0 <= x <= 1, 0 <= y <= f(x), where f(x) = x * x.
*/

// The algorithm is shown in "1.1.3 Linearization and Extrapolation"
int num_of_result;// this variable stores the numerator of the delta_area
int den_of_result;// this variable stores the denominator of the delta_area

int num_of_sum;// this variable stores the numerator of current sum
int den_of_sum;// this variable stores the denominator of current sum

int gcd(int,int);// calculates the greatest common divisor of its input
int mod(int,int);
void plus(int,int,int,int);
void delta_area(int,int,int,int,int,int);
int num_of_f(int);
int den_of_f(int);
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
void foo(int den){
	// This function calculates the approximate value of the area.
    //assume(den >= 3 && den <= 20);
		
	int num;
	// h is 1 / den
	
	num_of_sum = 0;
	den_of_sum = 1;
	for(num = 0;num < den;num++){
		delta_area(num_of_f(num),den_of_f(den),num_of_f(num + 1),den_of_f(den),1,den);
		plus(num_of_result,den_of_result,num_of_sum,den_of_sum);		
	}
	assert(den_of_sum <= num_of_sum * 4 && den_of_sum >= num_of_sum * 2);		
	//printf("num_of_sum:%d den_of_sum:%d\n",num_of_sum,den_of_sum);	
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

void plus(int num1,int den1,int num2,int den2){
	/* 
		This function adds two fractions
		and the result of num1 / den1 + num2 / den2 is stored in 
		the variable "num_of_sum" and "den_of_sum"
	*/
	int num = num1 * den2 + num2 * den1;
	int den = den1 * den2;
	int d = gcd(num,den);
	num_of_sum = num / d;
	den_of_sum = den / d;
}

int num_of_f(int a){
	// f(x) = x * x
	// This function calculates the numerator of f(a/b) and returns it.
	return a * a;
}

int den_of_f(int b){
	// f(x) = x * x
	// This function calculates the denominator of f(a/b) and returns it.
	return b * b;
}

void delta_area(int num1,int den1,int num2,int den2,int num_delta,int den_delta){
	/*
		This function calculates the area of a small part of the specified region,
		and the result is stored in the
		variable "num_of_result" and "den_of_result"
	*/
	int num = (num1 * den2 + num2 * den1) * num_delta;
	int den = 2 * den1 * den2 * den_delta;
	int d = gcd(num,den);
	num_of_result = num / d;
	den_of_result = den / d;	
}
