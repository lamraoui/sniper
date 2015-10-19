

int main(int x) {


    __CPROVER_assume(x==1);
    
    int y = 2;
    int z = 3;
    
    int ret = x+y+z;
    assert(ret<4);
    return ret;
}