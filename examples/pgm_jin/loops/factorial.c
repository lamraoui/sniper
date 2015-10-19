/*
 pre:  x is a nonnegative integer
 post: fac returns the factorial value of x
 in the following program, we just check whether fac(5) == 120
 */


int fac(int);
int fac2(int n, int result);

void foo(int x){
    assume(x == 5);
    int y = fac(x);
    assert(y == 120);
    return;
}

int fac(int n){
    if(n == 0 || n == 1)
        return 1;
    return n * fac(n - 1);
}

int fac2(int n, int result) {
    if (n < 2)
        return result;
    else 
        return fac2(n-1,result*n);
}