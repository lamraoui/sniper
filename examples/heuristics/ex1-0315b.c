/*
  pre:  x >= 0
  post: ((x =< 1) implies (y > 5)) and ((1 < x) implies (y <= 1)) 
*/

//# define implies(a, b)    !(a) || (b)
#define false 0
#define implies ==false ||
//#define implies ^1 ?  1 :

int y;

void main (int x) {
  int a, b, u, v;

    
    // PRE
    assert(x==9);
    //__CPROVER_assume(x>=0);
    
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
    
    if (v < 2) {   // BUG
        y = u + v;
    } else {
        y = v - u;
    }
    
    //POST ((x =< 1) implies (y > 5)) and ((1 < x) implies (y <= 1)) 
    //assert(((x <= 1) implies (y > 5)) && ((1 < x) implies (y <= 1))); 
    assert(((x <= 1) implies (y > 5)));
    assert(((1 < x) implies (y <= 1)));
    
}

