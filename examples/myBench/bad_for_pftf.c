/*
 18-02-2015
 
 An example of program that does not work with PFTF encoding
 but works with FFTF encoding.
 
 Result with FFTF:
 
 -- EI (= i 0)  : {{24}, {25}}
 -- {{24*, 25}}
 
 Result with PFTF:
 
 -- Partial part of TF: (and (or (and (= i -1) (= tmp6 1) bb4_bb7) (not bb4)) (or bb13_bb15 (not bb13)))
 -- EI (= i 0)  : {{25}}
 -- {{25}}
 
 
 */
extern void assert(int x);

int foo(int i) {
    int r;
    if (i>=0) { // Bug: should be i>0
        r = 0 + (i-i);
    } else {
        r = 1 + (i-i);
    }
    assert( (i<=0 && r==1) || (i>0 && r==0) );
    return r;
}