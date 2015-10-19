
#define LEN 3

extern void assert(int x);


void foo(int x) {

    int a = x;
    int c[LEN];
    
    int i = 0;
    for (i=0; i<LEN; ++i) {
        c[i] = -a;
    }
    
    assert(c[0]==x && c[1]==x && c[2]==x);
}
