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
    
    //int M[5];
    int M_0 = 1;
    int M_1 = 2;
    int M_2 = 4;
    int M_3 = 8;
    int M_4 = 16;
    
    
    int bigger = 0;
    
    int i = 0;
    // LOOP 0
    if (i<5) {
        if (x>M_0 && i==0) {
            bigger++;
        }
        i++;
        // LOOP 1
        if (i<5) {
            if (x>M_1 && i==1) {
                bigger++;
            }
            i++;
            // LOOP 2
            if (i<5) {
                if (x>M_2 && i==2) {
                    bigger++;
                }
                i++;
                // LOOP 3
                if (i<5) {
                    if (x>M_3 && i==3) {
                        bigger++;
                    }
                    i++;
                    // LOOP 4
                    if (i<5) {
                        if (x>M_4 && i==4) {
                            bigger--; // BUG: bigger++;
                        }
                        i++;
                        // LOOP 5
                        if (i<5) {
                           
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
           || (x>16 && bigger==5)
           );
    return bigger;
}

