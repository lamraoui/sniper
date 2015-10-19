// Mult
// z = x * y
//
int main(int x, int y)
{
    assert(x==5);
    assert(y>0);
    
    int z = 0;
    
    int i;
    for (i = 0; i < x; i++) {
        z = z - y;
    }
    
    assert(z==5*y);
    
    return 0;
}
