/*
#include <stdio.h>
int main() {
    int b = foo(65);
    printf("%d\n", b);
}
 */

// Compute the number of value in M 
// that are bigger than x
int foo(int x) 
{
    
    //int M[10];
    int M_0 = 1;
    int M_1 = 2;
    int M_2 = 4;
    int M_3 = 8;
    int M_4 = 16;
    int M_5 = 32;
    int M_6 = 64;
    int M_7 = 128;
    int M_8 = 256;
    int M_9 = 512;
    
    
    int bigger = 0;
    
    int i = 0;
    // LOOP 0
    if (i<10) {
        if (x>M_0 && i==0) {
            bigger++;
        }
        i++;
        // LOOP 1
        if (i<10) {
            if (x>M_1 && i==1) {
                bigger++;
            }
            i++;
            // LOOP 2
            if (i<10) {
                if (x>M_2 && i==2) {
                    bigger++;
                }
                i++;
                // LOOP 3
                if (i<10) {
                    if (x>M_3 && i==3) {
                        bigger++;
                    }
                    i++;
                    // LOOP 4
                    if (i<10) {
                        if (x>M_4 && i==4) {
                            bigger++;
                        }
                        i++;
                        // LOOP 5
                        if (i<10) {
                            if (x>M_5 && i==5) {
                                bigger--; // BUG: bigger++;
                            }
                            i++;
                            // LOOP 6
                            if (i<10) {
                                if (x>M_6 && i==6) {
                                    bigger++;
                                }
                                i++;
                                // LOOP 7
                                if (i<10) {
                                    if (x>M_7 && i==7) {
                                        bigger++;
                                    }
                                    i++;
                                    // LOOP 8
                                    if (i<10) {
                                        if (x>M_8 && i==8) {
                                            bigger++;
                                        }
                                        i++;
                                        // LOOP 9
                                        if (i<10) {
                                            if (x>M_9 && i==9) {
                                                bigger++;
                                            }
                                            i++;
                                            // LOOP 10
                                            if (i<10) {
                                                
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    
    assert( (x<=1 && bigger==0) 
           || (x==2 && bigger==1) 
           || (x>2 && x<=4 && bigger==2)
           || (x>4 && x<=8 && bigger==3) 
           || (x>8 && x<=16 && bigger==4) 
           || (x>16 && x<=32 && bigger==5)
           || (x>32 && x<=64 && bigger==6) 
            || (x>64 && x<=128 && bigger==7)
            || (x>128 && x<=256 && bigger==8) 
            || (x>256 && x<=512 && bigger==9) 
            || (x>512 && bigger==10) 
           );
    return bigger;
}

