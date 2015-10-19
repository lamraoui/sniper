/*
 post: a[0]...a[len - 1] is a sorted array
 
 http://en.wikipedia.org/wiki/Selection_sort
 */

#define LEN 10

extern void assert(int x);

void foo(int x0, int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8, int x9){
    
    int a0 = x0;
    int a1 = x1;
    int a2 = x2;
    int a3 = x3;
    int a4 = x4;
    int a5 = x5;
    int a6 = x6;
    int a7 = x7;
    int a8 = x8;
    int a9 = x9;
    
    /* a[0] to a[n-1] is the array to sort */
    int i,j;
    int iMin;
    
    /* advance the position through the entire array */
    /*   (could do j < n-1 because single element is also min element) */
    for (j = 0; j < LEN-1; j++) { // BUG LEN-1
        /* find the min element in the unsorted a[j .. n-1] */
        
        /* assume the min is the first element */
        iMin = j;
        /* test against elements after j to find the smallest */
        for ( i = j+1; i < LEN; i++) {
            
            // ai = a[i];
            int ai;
            if (i==0) {
                ai = a0;
            } else if (i==1) {
                ai = a1;
            } else if (i==2) {
                ai = a2;
            } else if (i==3) {
                ai = a3;
            } else if (i==4) {
                ai = a4;
            } else if (i==5) {
                ai = a5;
            } else if (i==6) {
                ai = a6;
            } else if (i==7) {
                ai = a7;
            } else if (i==8) {
                ai = a8;
            } else if (i==9) {
                ai = a9;
            }
            
            // aiMin = a[iMin];
            int aiMin;
            if (iMin==0) {
                aiMin = a0;
            } else if (iMin==1) {
                aiMin = a1;
            } else if (iMin==2) {
                aiMin = a2;
            } else if (iMin==3) {
                aiMin = a3;
            } else if (iMin==4) {
                aiMin = a4;
            } else if (iMin==5) {
                aiMin = a5;
            } else if (iMin==6) {
                aiMin = a6;
            } else if (iMin==7) {
                aiMin = a7;
            } else if (iMin==8) {
                aiMin = a8;
            } else if (iMin==9) {
                aiMin = a9;
            }
            
            // if this element is less, then it is the new minimum
            if (ai < aiMin) {
                // found new minimum; remember its index
                iMin = i;
            }
        }
        
        if(iMin != j) {
            
            // swap(a[j], a[iMin]);
            
            // tmp = a[j];
            int aj;
            if (j==0) {
                aj = a0;
            } else if (j==1) {
                aj = a1;
            } else if (j==2) {
                aj = a2;
            } else if (j==3) {
                aj = a3;
            } else if (j==4) {
                aj = a4;
            } else if (j==5) {
                aj = a5;
            } else if (j==6) {
                aj = a6;
            } else if (j==7) {
                aj = a7;
            } else if (j==8) {
                aj = a8;
            } else if (j==9) {
                aj = a9;
            }
            
            // aiMin = a[iMin];
            int aiMin;
            if (iMin==0) {
                aiMin = a0;
            } else if (iMin==1) {
                aiMin = a1;
            } else if (iMin==2) {
                aiMin = a2;
            } else if (iMin==3) {
                aiMin = a3;
            } else if (iMin==4) {
                aiMin = a4;
            } else if (iMin==5) {
                aiMin = a5;
            } else if (iMin==6) {
                aiMin = a6;
            } else if (iMin==7) {
                aiMin = a7;
            } else if (iMin==8) {
                aiMin = a8;
            } else if (iMin==9) {
                aiMin = a9;
            }
            
            
            // a[j] = aiMin;
            if (j==0) {
                a0 = aiMin;
            } else if (j==1) {
                a1 = aiMin;
            } else if (j==2) {
                a2 = aiMin;
            } else if (j==3) {
                a3 = aiMin;
            } else if (j==4) {
                a4 = aiMin;
            } else if (j==5) {
                a5 = aiMin;
            } else if (j==6) {
                a6 = aiMin;
            } else if (j==7) {
                a7 = aiMin;
            } else if (j==8) {
                a8 = aiMin;
            } else if (j==9) {
                a9 = aiMin;
            }
            
            
            // a[iMin] = aj;
            if (iMin==0) {
                a0 = aj;
            } else if (iMin==1) {
                a1 = aj;
            } else if (iMin==2) {
                a2 = aj;
            } else if (iMin==3) {
                a3 = aj;
            } else if (iMin==4) {
                a4 = aj;
            } else if (iMin==5) {
                a5 = aj;
            } else if (iMin==6) {
                a6 = aj;
            } else if (iMin==7) {
                a7 = aj;
            } else if (iMin==8) {
                a8 = aj;
            } else if (iMin==9) {
                a9 = aj;
            }

            
        }
        
    }
    assert(a0<=a1 && a1<=a2 && a2<=a3 && a3<=a4 && a4<=a5 && a5<=a6 && a6<=a7 && a8<=a9);
}
