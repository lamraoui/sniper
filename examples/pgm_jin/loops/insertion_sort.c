/*
 pre:  len is a positive integer
 post: a[0]...a[len - 1] is a sorted array
 */

int a[20];

int mod(int,int);
void assign(int);
int check(int);
void insertion_sort(int);

void foo(int len){
    assume(len == 10);
    assign(len);
    insertion_sort(len);	
    int r = check(len);
    assert(r == 1);
}

void assign(int len){
    // this function assigns arbitrary value to the a[0]...a[len - 1]
    int temp = 2;
    for(int i = 0;i < len;i++){
        temp = temp * 11 + 13;
        temp = mod(temp,31);
        a[i] = temp;
    }	
}

void insertion_sort(int len){
    // sort the array a[0]...a[len - 1] using insertion sort
    int key;
    for(int i = 1;i < len;i++){
        key = a[i]; 
        int j = i - 1;
        while(j >= 0 && a[j] > key){
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

int check(int len){
    // check whether the array (len is its length) is a sorted array
    for(int i = 1;i < len;i++){
        if(a[i] < a[i - 1])
            return 0;
    }
    return 1;
}

int mod(int a,int b){
    // since the "%" operator is still not fully supported in Sniper
    // so I just write a simple mod function to replace "%" operator.
    // requirement: a >= 0,b > 0
    while(a >= b)
        a -= b;
    return a;
}