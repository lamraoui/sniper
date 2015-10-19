
extern void assert(int x);

void foo(int degree) {
    int d;
    if (degree==0) {
        d = 0; // torque
    } else if (degree>0) {
        d = 1; // rotate right
    } else {
        d = 2; // rotate left
    }
    int r = (d==1)?degree*-1:degree;
    
    assert(0 <= d <= 2 && r >= 0);
    //motor(r, d);
}