/*
 pre:  len is a positive integer
 post: a[0]...a[len - 1] is a sorted array
 
 for i ← 1 to length(A)
    x ← A[i]
    j ← i
    while j > 0 and A[j-1] > x
        A[j] ← A[j-1]
        j ← j - 1
    A[j] ← x
 
 http://en.wikipedia.org/wiki/Insertion_sort
 */
#define LEN 3

extern void assert(int x);

void foo(int a0, int a1, int a2){
    
    // sort the array a[0]...a[len - 1] using insertion sort
    int key;
    for(int i = 1;i < LEN;i++) {
        
        // key = a[i];
        if (i==0) {
            key = a0;
        } else if (i==1) {
            key = a1;
        } else if (i==2) {
            key = a2;
        }
        
        int j = i;
        
        // tmp = a[j-1];
        int tmp;
        if (j-1==0) {
            tmp = a0;
        } else if (j-1==1) {
            tmp = a1;
        } else if (j-1==2) {
            tmp = a2;
        }
        
        while(j > 0 /*&& tmp > key*/){ //a[j-1]>key
            
            // a[j] = tmp;
            if (j==0) {
                a0 = tmp;
            } else if (j==1) {
                a1 = tmp;
            } else if (j==2) {
                a2 = tmp;
            }
            
            j = j - 1;
            
            // tmp = a[j-1];
            int tmp;
            if (j-1==0) {
                tmp = a0;
            } else if (j-1==1) {
                tmp = a1;
            } else if (j-1==2) {
                tmp = a2;
            }
            if (tmp > key) {
                break;
            }
        }
        
        //a[j] = key;
        if (j==0) {
            a0 = key;
        } else if (j==1) {
            a1 = key;
        } else if (j==2) {
            a2 = key;
        }
    }
    
    assert(a0<=a1 && a1<=a2);
}
