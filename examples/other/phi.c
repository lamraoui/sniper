
int foo(int input) {

    int x = 0;
    if (input>20) {
        x = 1;
    } else if (input>10) {
        x = 2;
    } else {
        x = 3;
    }
    
    int y = 0;
    if (x==1) {
        y = 100; // input = 1
    } else if (x==2) {
        y = 200; // input = 11
    } else {
        y = input; // input = 0
    }

    assert(y<99);
    return y;
}