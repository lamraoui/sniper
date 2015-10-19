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

void foo(int x0, int x1, int x2){
    
    
    int a[LEN];
    a[0] = x0;
    a[1] = x1;
    a[2] = x2;
    
    // sort the array a[0]...a[len - 1] using insertion sort
    int key;
    for(int i = 1;i < LEN;i++) {
        
        key = a[i];
        int j = i;
        
        //int tmp = a[j-1];
        //while(j > 0 /*&& tmp > key*/){ //a[j-1]>key
        while(j > 0 && a[j-1]>key){ //
            
            //a[j] = tmp;
            a[j] = a[j-1];
            j = j - 1;
            
            /*tmp = a[j-1];
            if (tmp > key) {
                break;
            }*/
        }
        
        a[i] = key; // Bug: should be: a[j] = key;
    }
    
    assert(a[0]<=a[1] && a[1]<=a[2]);
}
