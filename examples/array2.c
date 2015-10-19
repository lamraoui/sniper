
int main(int y) {
    
    int a[4];
    int b[2];
    int c[10];
    a[0] = 100+y;
    b[1] = a[0];
    c[5] = b[1];
   

    int x;
    x = c[5];
    
    assert(x==100);
    return x;
}
