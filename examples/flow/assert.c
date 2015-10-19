
int foo(int input) {
    
    int out;
    if (input>0) {
        out = 1;
    } else {
        out = 2;
    }

    
    assert((input==0 && out==0) || (input!=0 && out==3));
    
    return out;
}
