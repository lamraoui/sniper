/*
 pre:  n is a nonnegative integer
 post: the return value of Fib(n) is Fibonacci[n];
 */

int Fib(int);

void foo(int x){
    //assume(x>=0 && x<=5);
    //assume(x==5);
    
    int y = 0;
    if (x>=0 && x<=5) {
        y = Fib(x);
    } 
    //assert(y == 5);
    assert( (x==0 && y==0)
           || (x==1 && y==1)
           || (x==2 && y==1)
           || (x==3 && y==2)
           || (x==4 && y==3)
           || (x==5 && y==5));
    return;
}

int Fib(int n){
    if(n == 0 || n == 1)
        return n;
    int f0 = 0;
    int f1 = 1;
    for(int i = 2;i <= n;i++){
        int a = f1;
        int b = f0 - f1;
        f0 = a;
        f1 = b;
    }
    return f1;
}
