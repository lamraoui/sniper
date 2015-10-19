// Middle number

/*
 
 BugAssist:
 < 1 > Potential BUG @ :./ex/middle.c:27 : m = z;
 < 3 > Potential BUG @ :./ex/middle.c:48 : assert(m==2);
 < 4 > Potential BUG @ :./ex/middle.c:23 : if (y<z) {
 
*/

int main(int x, int y, int z) {

    int m;
    
    // TEST 1 
    //assert(x==3); assert(y==3); assert(z==5);
    // TEST 2
    //assert(x==1); assert(y==2); assert(z==3); 
    // TEST 3
    //assert(x==3); assert(y==2); assert(z==1);
    // TEST 4
    //assert(x==5); assert(y==5); assert(z==5);
    // TEST 5
    //assert(x==5); assert(y==3); assert(z==4);
    // TEST 6
    assert(x==2); assert(y==1); assert(z==3);
    
    
    m = z;
    if (y<z) {
        if (x<y) {
            m = y; 
        } else if(x<z) {
            m = y; // m = x; ## bug ##
        }
    } else {
        if (x>y) {
            m = y;
        } else if(x>z) {
            m = x;
        }
    }
    
    // TEST 1
    //assert(m==3);
    // TEST 2
    //assert(m==2);
    // TEST 3
    //assert(m==2);
    // TEST 4
    //assert(m==5);
    // TEST 5
    //assert(m==3);
    // TEST 6
    assert(m==2);
    
    return m;
}