void foo(int x, int z) {
    int y = z;
    if (x > y) {
        y = x;
    }
    x = y;
    assert((y < z) && (x > z));
}