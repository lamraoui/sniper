
int foo(int x) {

    int y = 40;
    if (x==0) {
        y = y + x + 2;
    } else if (x==1) {
        y = y + x + 2; // BUG
    } else {
        y = y + 1; // BUG
    }
    assert(y==42);
}