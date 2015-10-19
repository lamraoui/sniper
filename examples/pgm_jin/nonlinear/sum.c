/*
   pre:  n is a positive integer with a small value(less than 20)
   post: the function foo calculates the sum of a[0]...a[n]
*/

int a[20];

int sum(int,int);
void assign(int);
/*
#include <stdio.h>
#include <assert.h>

void foo(int n);
int main(){
	int i;
	for(i = 1;i < 20;i++)
		foo(i);
	return 0;
}
*/
void foo(int n){
	// This function calculates the sum of a[0]...a[n] which is an arithmetic progression.
    assume(n > 0 && n < 20);
	assign(n);
	assert(sum(0,n) == (n + 1) * n / 2);
}

void assign(int n){
	// This function assigns values to a[0]...a[n] to make it become an arithmetic progression.
	int i;	
	for(i = 0;i <= n;i++)
		a[i] = i;
}

int sum(int low,int high){
	// This function calculates the sum of a[low]...a[high] by using the divide and conquer strategy.
	if(low > high)
		return 0;
	if(low == high)
		return a[low];
	int mid = (low + high) / 2;
	return sum(low,mid) + sum(mid + 1,high);	
}
