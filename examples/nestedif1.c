
int foo(int x) {

    int y = 0;
    int z = 0;
    
    //assert(x==2);
    //assert(z==249);
    
    
    if (x==0) {		// # ERROR
        x = x + 10;
        if (x<100) {
            z = 1;
        } else {
            z = 0;
        }
        if (z==0) {
            y = x * 2;
        } else {
            y = x / 2;
        }
    } else {
        y = z * 4;
    }
    
    assert(y<200);
    
    
    return 0;
}

