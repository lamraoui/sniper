
#define LEN 10

extern void assert(int x);


void foo(int x0, int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9,
         int y0, int y1, int y2, int y3, int y4, int y5, int y6, int y7, int y8, int y9){

    int a[LEN];
    a[0] = x0;
    a[1] = x1;
    a[2] = x2;
    a[3] = x3;
    a[4] = x4;
    a[5] = x5;
    a[6] = x6;
    a[7] = x7;
    a[8] = x8;
    a[9] = x9;
    
    int b[LEN];
    b[0] = y0;
    b[1] = y1;
    b[2] = y2;
    b[3] = y3;
    b[4] = y4;
    b[5] = y5;
    b[6] = y6;
    b[7] = y7;
    b[8] = y8;
    b[9] = y9;
    
    int c[LEN];
    
    int i;
    for (i=0; i<LEN; ++i) {
        c[i] = a[i]-b[i]; // BUG: +
    }
    
    assert(c[0]==(x0+y0) && c[1]==(x1+y1) && c[2]==(x2+y2) && c[3]==(x3+y3) && c[4]==(x4+y4) && c[5]==(x5+y5) && c[6]==(x6+y6) && c[7]==(x7+y7) && c[8]==(x8+y8) && c[9]==(x9+y9));
}
