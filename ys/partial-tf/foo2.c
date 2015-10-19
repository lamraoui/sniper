int foo2(int x) { 
    int y = 0; 
    if(x>0) {
        y = 42;
    } else { 
        for(int i=0; i<5; i++) {
            y = y + 1; // BUG
        }
    }
    assert (y<=5); 
    return y;
}