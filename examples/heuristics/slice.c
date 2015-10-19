// Slice

void main(int a, int b) {
//void main() { int a,b;
    
    
    // CE
    assert(a==1073741824);
    assert(b==1073741824);
    
    int x = 1;
    int y = 1;
    int z = 1;
    if (a>0) {
        x = x + 5;
        y = y + 6;
        z = z + 4;
    }
    if (b>0) {
        x = x + 6;
        y = y + 5;
        z = z + 4;
    }
    assert((x<10) || (y<10));
}