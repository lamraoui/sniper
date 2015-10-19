
int foo(int x) 
{
    int y = 0;
    if (x>0) {
        if (x==1) {
            for (int i=0; i<100; ++i) {
                y = y + x;
            }
        } else {
            y = 100;
        }
    } else {
        if (x==-100) {
            y = x+200;
        } else {
            y = 1; // BUG
        }
    }
    assert(y==100);
    return y;
}

