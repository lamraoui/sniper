/*
 pre:  low and high are both nonnegative integers, and low <= high
 post: a[low]...a[high] is a sorted array
 */

int a[20];
int a1[20];
int a2[20];

const int inf = 10 * 10000 * 10000;

int mod(int,int);
void assign(int);
int check(int);
void merge_sort(int,int);
void merge(int,int,int);

void foo(int x){
    assume(x == 10);
    assign(x);
    merge_sort(0,x - 1);	
    assert(check(x) == 1);
}

void assign(int len){
    // this function assigns arbitrary value to the array a[0]...a[len - 1]
    int temp = 2;
    for(int i = 0;i < len;i++){
        temp = temp * 11 + 13;
        temp = mod(temp,31);
        a[i] = temp;
    }	
}

void merge_sort(int low,int high){
    if(low >= high)
        return;
    int mid = (low + high) / 2;
    merge_sort(low,mid);
    merge_sort(mid + 1,high);
    merge(low,mid,high);
}

void merge(int low,int mid,int high){
    int n1 = mid - low + 1;
    int n2 = high - mid;
    for(int i = 1;i <= n1;i++)
        a1[i] = a[low + i - 1];
    for(int j = 1;j <= n2;j++)
        a2[j] = a[mid + j];
    a1[n1 + 1] = inf;
    a2[n2 + 1] = inf;
    int i = 1;
    int j = 1;
    for(int k = low;k <= high;k++){
        if(a1[i] <= a2[j]){
            a[k] = a1[i++];
        } else {
            a[k] = a2[j++];
        }
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