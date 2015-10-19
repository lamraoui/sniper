// Mult
// z = x * y
//
void main(int x)
{
    assert(x>0);
    
    int tmp = x;
    
    while (tmp>0) {
        tmp = tmp + 1;
    }
    
    assert(tmp==0);
}
