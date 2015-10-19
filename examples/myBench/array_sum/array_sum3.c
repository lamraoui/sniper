
#define LEN 3

extern void assert(int x);


void foo(int x0, int x1, int x2, int y0, int y1, int y2){

    int a[LEN];
    a[0] = x0;
    a[1] = x1;
    a[2] = x2;
    
    int b[LEN];
    b[0] = y0;
    b[1] = y1;
    b[2] = y2;
    
    int c[LEN];
    
    int i;
    for (i=0; i<LEN; ++i) {
        c[i] = a[i]-b[i]; // BUG: +
    }
    
    
    //assert(c[0]==(a[0]+b[0]) && c[1]==(a[1]+b[1]) && c[2]==(a[2]+b[2]));
    assert(c[0]==(x0+y0) && c[1]==(x1+y1) && c[2]==(x2+y2));
}
