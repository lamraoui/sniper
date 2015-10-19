// Foo3

void main(int x, int y) {
   
    assert(x==1);
    assert(y==1);
    
    int a = x + y; // BUG : x - y
    
    int z;
    if (x < y) {
        z = a;
    } else {
        z = a + 1;
    }
    assert(z==1);
}