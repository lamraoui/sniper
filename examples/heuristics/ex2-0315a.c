/*
 pre  : k >= 0
 post : y > 1
 */

int y;

void main () { int i;int j;int k;
//void main (int i, int j, int k) {
    int x;

    //PRE  
    assert(k==5);
    //__CPROVER_assume(k >= 0);
    
    
    if(k >= 5) {
        i = 1; 
        j = 2;
    } else if (3 <= k < 5) {
        i = 2; 
        j = 0;
    } else if (1 <= k < 3) {
        i = 0; 
        j = 1;
    } else {
        i = 0; 
        j = 0;
    }
    
    x = i + j - 1; /* should be i + j - 2 */
    
    if(3*x > k) {
        y = 3*x - k;
    } else {
        y = k - 2*x - 2;
    }
    
    //POST
    assert(y>1);
}

