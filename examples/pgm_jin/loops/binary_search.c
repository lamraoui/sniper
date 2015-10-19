/*
 pre:  value is an integer
 post: (return value of binary_search) implies (input parameter "value" can be found in a[low]...a[high])
 and (return value of binary_search) implies (input parameter "value" cannot be found in a[low]..a.a[high])
 */

int a[20];

int binary_search(int,int,int);
void assign(int,int);

void foo(int low,int high,int value){
    assume(low == 1);
    assume(high == 10);
    assume(value == 18);
    
    assign(low,high);
    
    int found = binary_search(low,high,value);
    assert(found < 0);// 18 can not be found in a[1]...a[10]
}

void assign(int low,int high){
    // this function assigns some arbitrary value to the array a[] (in ascending order).	
    for(int i = low;i <= high;i++)
        a[i] = 5 * i + 2;	
}


int binary_search(int low,int high,int value){
    int mid;
    while(low <= high){
        mid = (low + high) / 2;
        if(value == a[mid])
            return 1;
        if(value > a[mid])
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}