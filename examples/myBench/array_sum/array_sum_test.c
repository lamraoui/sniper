
#define LEN 10

extern void assert(int x);


void foo(int x, int y) {

    int a = x;
    int b = y;
    int c = 0;
    int i = 0;
    for (i=0; i<LEN; ++i) {
        c = c + (a - b); // BUG: +
    }
    
    assert(c==10);
}
