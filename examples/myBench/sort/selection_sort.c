/*
 post: a[0]...a[len - 1] is a sorted array
 
 http://en.wikipedia.org/wiki/Selection_sort
 */

#define LEN 3

extern void assert(int x);


void foo(int x0, int x1, int x2){
    
    int a[LEN];
    a[0] = x0;
    a[1] = x1;
    a[2] = x2;
    
    // a[0] to a[n-1] is the array to sort
    int i,j;
    int iMin;
    
    // advance the position through the entire array
    //   (could do j < n-1 because single element is also min element)
    for (j = 0; j < LEN-1; j++) {
        // find the min element in the unsorted a[j .. n-1]
        
        // assume the min is the first element
        iMin = j;
        // test against elements after j to find the smallest
        for ( i = j+1; i < LEN; i++) {
            // if this element is less, then it is the new minimum
            if (a[i] < a[iMin]) {
                // found new minimum; remember its index
                iMin = i;
            }
        }
        if(iMin == j) { // BUG: iMin != j
            // swap(a[j], a[iMin]);
            int tmp = a[j];
            a[j] = a[iMin];
            a[iMin] = tmp;
        }
    }
    
    assert(a[0]<=a[1] && a[1]<=a[2]);
}
