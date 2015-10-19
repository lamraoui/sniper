
int foo(int x) {
    
    int y = 0;
    if (x==1) {
        // 42 times y++
        y = y + x;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
        y = y + 1;
    } else {
        y = x - 42;  // BUG
    }
    assert(y==42);
}