void foo(int x) {
    int a;
    if(x>0) {
        a = 1;
    } else {
        a = 10;
    }
    assert((x<=0 && a==0) || (x>0 && a==10));
}