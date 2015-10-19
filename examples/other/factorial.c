
int factorial_impl(int v, int p)
{
    if (v == 0 || v ==1) return p;
    else return factorial_impl(v-1,p*v);
}
int factorial (int v)
{
    return factorial_impl(v,1);
}

/*int factorial(int n){
    //assume(n >= 0 && n <= 10);
    if(n == 0 || n == 1)
        return 1;
    return factorial(n - 1) * n;
}*/

int foo(int x){
    assume(x == 3);
    int y = factorial(x);
    assert(y == 6);
    return 0;
}

