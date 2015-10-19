
int main(int y) {
    
    int a[4];

    a[3] = 42;
    
    int x = a[y+1];
    
    
    
    assert(x==42);
    return x;
}
