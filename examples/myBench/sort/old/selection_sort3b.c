/*
 post: a[0]...a[len - 1] is a sorted array
 
 http://en.wikipedia.org/wiki/Selection_sort
 */

#define LEN 3

extern void assert(int x);

void foo(int a0, int a1, int a2){
    
    /* a[0] to a[n-1] is the array to sort */
    int i,j;
    int iMin;
    
    /* advance the position through the entire array */
    /*   (could do j < n-1 because single element is also min element) */
    for (j = 0; j < LEN-1; j++) {
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
            }
            
            // aiMin = a[iMin];
            int aiMin;
            if (iMin==0) {
                aiMin = a0;
            } else if (iMin==1) {
                aiMin = a1;
            } else if (iMin==2) {
                aiMin = a2;
            }
            
            /* if this element is less, then it is the new minimum */
            if (ai > aiMin) { // BUG: <
                /* found new minimum; remember its index */
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
            }
            
            // aiMin = a[iMin];
            int aiMin;
            if (iMin==0) {
                aiMin = a0;
            } else if (iMin==1) {
                aiMin = a1;
            } else if (iMin==2) {
                aiMin = a2;
            }
            
            
            // a[j] = aiMin;
            if (j==0) {
                a0 = aiMin;
            } else if (j==1) {
                a1 = aiMin;
            } else if (j==2) {
                a2 = aiMin;
            }
            
            
            // a[iMin] = aj;
            if (iMin==0) {
                a0 = aj;
            } else if (iMin==1) {
                a1 = aj;
            } else if (iMin==2) {
                a2 = aj;
            }

            
        }
        
    }
    assert(a0<=a1 && a1<=a2);
}
