
int foo(int x) 
{
    int y = 0;
    if (x>0) {
        if (x==10) {
            y = x-10;
        } else {
            y = 0;
        }
    } else {
        if (x==-10) {
            y = x+10;
        } else {
            y = 1; // BUG
        }
    }
    assert(y==0);
    return y;
}

