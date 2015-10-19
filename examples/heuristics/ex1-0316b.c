/*
  pre:  x >= 0
  post: ((x <= 1) implies (y > 5)) 
    and ((1 < x <= 6) implies (y <= 1))
    and ((6 < x < 10) implies (y >= 6))
    and ((10 <= x) implies (y <= 0)) 
*/

#define false 0
#define implies ==false ||

int y;

//int main (int x) {
int foo(int x) {
    int a, b, u, v;
    
    
    //assert(x==2147483647);
    __CPROVER_assume(x>=0);
    
    if (x < 10) {
        a = 4; 
        b = 1;
    } else {
        a = 1; 
        b = 4;
    }
    
    if (x + a > 10) {
        u = a + b; 
        v = b;
    } else if (x + a <= 5) {
        u = a + b; 
        v = x + b;
    } else {
        u = x + b; 
        v = a;
    }
    
    if (v < 2) {
        y = u + v;
    } else {
        y = v - u;
    }
    
    assert( ((x <= 1) implies (y > 5)) );
    assert( ((1 < x <= 6) implies (y <= 1)) );
    assert( ((6 < x < 10) implies (y >= 6)) );
    assert( ((10 <= x) implies (y <= 0)) );
    
    /*assert( ((x <= 1) implies (y > 5)) 
           && ((1 < x <= 6) implies (y <= 1)) 
           && ((6 < x < 10) implies (y >= 6)) 
           && ((10 <= x) implies (y <= 0)) );*/
    
    return(y);
}

