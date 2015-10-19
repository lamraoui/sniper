extern void assert(int x);


// Type of faults: CDF
// Number of faults: 2
void foo1(int x) {
    int y = 1;
    if (x==1) { // should be x==0
        y = -1; // should be y=0
    }
    assert(y>0);
}
// FFTF
// -- Error-inducing Input: (= x 1)
// MCSes: {{9}, {8}}
// Combined MCSes: {{8, 9}}

// Type of faults: CIF (two inputs mandatory)
// Number of faults: 2
void foo2(int x) {
    int y = 1;
    if (x>0) {
        y = 0; // should be y=1
    } else {
        y = 1; // should be y=0
    }
    assert( (x>0 && y==1) || (x<=0 && y==0) );
}
// FFTF
// -- Error-inducing Input: (= x 1)
// MCSes: {{9}, {8}}
// Combined MCSes: {{8, 9}}
// -- Error-inducing Input: (= x 0)
// MCSes: {25}, {22}}
// -- Error-inducing Input: (= x 1)
// MCSes: {{23}, {22}}
// Combined MCSes: {{22, 23, 25}}

// Type of faults: CIF (one input is sufficient)
// Number of faults: 2
void foo3(int x) {
    int y = 1;
    if (x>0) {
        y = 0; // should be y=1
    } else {
        y = 0; // should be y=2
    }
    assert(y>0);
}
// FFTF
// -- Error-inducing Input: (= x 0)
// MCSes: {{46}, {43, 44}}
// -- Error-inducing Input: (= x 1)
// MCSes: {{44}, {43, 46}}
// Combined MCSes: {{43, 44, 46}}

// Type of faults: CDF (sequential-dependence)
// Number of faults: 2
void foo4(int x) {
    int y;
    int z = z + 1;
    y = 0; // should be y=1
    z = z + 1;
    z = z + 1;
    y = y - x; // should be y=x+1
    z = z + 1;
    assert(y==x+1 && z>0);
}
// FFTF
// -- Error-inducing Input: (= x 0)
// MCSes: {{65}, {62}}
// Combined MCSes: {{62, 65}}

// Type of faults: CDF (input-dependent)
// Number of faults: 2
void foo5(int x, int y) {
    int z = 1;
    if (x==1) {
        z = 0;
        if (y==1) {
            z = 0;
        }
    }
    assert(z>0);
}
// FFTF
// -- Error-inducing Input: (and (= x 1) (= y 0))
// MCSes: {{79}, {78}, {80, 81}}
// -- Error-inducing Input: (and (= x 1) (= y 1))
// MCSes: {{81}, {78}, {79, 80}}
// Combined MCSes: {{78, 79, 80, 81}}



