
int binary_search(int key)
{
    // Init array
    int a[16];
    int n = 16;
    int i;
    for (i=0; i<n; ++i) {
        a[i] = 42;
    }
    
    int first = 0;
    int last = n - 1;
    int middle = (first+last)/2;
    
    int found_val = 0;
    while( first <= last )
    {
        if (a[middle]<key) {
            first = middle + 1;    
        } else if (a[middle]==key) {
            // found at middle+1
            found_val = a[middle];
            break;
        } else {
            last = middle - 1;
        }
        middle = (first + last)/2;
    }
    // if ( first > last )
    //     printf("Not found! %d is not present in the list.\n", search);
    
    
    assert(found_val==0 || found_val==key);
    return found_val;
}