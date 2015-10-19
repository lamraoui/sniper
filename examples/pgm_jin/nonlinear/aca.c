/*
   pre:  
   post: The function foo will calculate the area of a specified region.
   		The specified region: 0 <= x <= 1, 0 <= y <= f(x), where f(x) = x * x.
*/

// The algorithm is shown in "1.1.3 Linearization and Extrapolation"


int gcd(int,int);// calculates the greatest common divisor of its input
int mod(int,int);
int num_of_f(int);
int den_of_f(int);

#include <stdio.h>
#include <assert.h>

void foo(int);
int main(){
	int i;
	for(i = 3;i < 10;i++)
		foo(i);
	return 0;
}

void foo(int den){
	// This function calculates the approximate area.
	//assume(den >= 3 && den < 10);	
    if (den < 3 || den >= 10) {
        return;
    }
    
    int num_of_result;// this variable stores the numerator of the delta_area
    int den_of_result;// this variable stores the denominator of the delta_area
    int num_of_sum;// this variable stores the numerator of current sum
    int den_of_sum;// this variable stores the denominator of current sum
    
	int num;
	num_of_sum = 0;
	den_of_sum = 1;
	for(num = 0;num < den;num++){

        int num1 = num_of_f(num);
        int den1 = den_of_f(den);
        int num2 = num_of_f(num + 1);
        int den2 = den_of_f(den);
        int num_delta = 1;
        int den_delta = den;
        /*
         This function calculates the area of a small part of the specified region,
         and the result is stored in the
         variable "num_of_result" and "den_of_result"
         */
        int n = (num1 * den2 + num2 * den1) * num_delta;
        int de = 2 * den1 * den2 * den_delta;
        int d = gcd(n,de);
        num_of_result = n / d;
        den_of_result = de / d;
        
        
        /* 
         This function adds two fractions
         and the result of num1 / den1 + num2 / den2 is stored in 
         the variable "num_of_sum" and "den_of_sum"
         */
        n = num_of_result * den_of_sum + num_of_sum * den_of_result;
        de = den_of_result * den_of_sum;	
        d = gcd(n,de);	
        num_of_sum = n / d;
        den_of_sum = de / d; 
	}	
	int temp_num = num_of_sum;
	int temp_den = den_of_sum;
	num_of_sum = 0;
	den_of_sum = 1;
	den *= 2;
	for(num = 0;num < den;num++){
        
        int num1 = num_of_f(num);
        int den1 = den_of_f(den);
        int num2 = num_of_f(num + 1);
        int den2 = den_of_f(den);
        int num_delta = 1;
        int den_delta = den;
        /*
         This function calculates the area of a small part of the specified region,
         and the result is stored in the
         variable "num_of_result" and "den_of_result"
         */
        int n = (num1 * den2 + num2 * den1) * num_delta;
        int de = 2 * den1 * den2 * den_delta;
        int d = gcd(n,de);
        num_of_result = n / d;
        den_of_result = de / d;
        
        /* 
         This function adds two fractions
         and the result of num1 / den1 + num2 / den2 is stored in 
         the variable "num_of_sum" and "den_of_sum"
         */
        n = num_of_result * den_of_sum + num_of_sum * den_of_result;
        de = den_of_result * den_of_sum;	
        d = gcd(n,de);	
        num_of_sum = n / d;
        den_of_sum = de / d;
	}
	
    /* 
     This function calculate the result of num1 / den1 - num2 / den2,
     the result is stored in 
     the variable "num_of_result" and "den_of_result"
     */
	int n = (4*num_of_sum) * temp_den - temp_num * den_of_sum;
	int de = den_of_sum * temp_den;
	int d = gcd(n,de);
	num_of_result = n / d;
	den_of_result = de / d;
    
    
    
	den_of_result *= 3;
	
	//printf("%d %d\n",num_of_result,den_of_result);
	sassert(den_of_result <= 4 * num_of_result && den_of_result >= 2 * num_of_result);	
}

/*int gcd(int a,int b){
	// This function calculates the greatest common divisor of a and b.
	// requirement of input: a > 0, b >= 0
	if(b == 0)
		return a;
	return gcd(b,mod(a,b));
}*/

int gcd(int a, int b) {
    int c;
    while ( a != 0 ) {
        c = a; a = b%a;  b = c;
        //printf("%d %d\n", a, b);
    }
    return b;
}

int mod(int a,int b){
	// This function calculates a % b.
	// requirement of input: a >= 0, b > 0
    return a % b;
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

