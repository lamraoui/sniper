
int main(int y) {
    
    int a[4];

    if (y>0 && y==11) {
        a[0] = 10*y;
    } else {
        a[0] = 100;
    }
    int x = a[0];
    
    int z = x;
    
    assert(z==100);
    return z;
}
