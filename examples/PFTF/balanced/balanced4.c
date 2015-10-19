
extern void assert(int x);

int foo(int x) {
    int y = 0;
    if (x>0) {
        if (x>0) {
            if (x>0) {
                if (x>0) {
                    y = x - 1; // BUG
                } else {
                    y = x + 1;
                }
            } else {
                if (x>0) {
                    y = x + 1;
                } else {
                    y = x + 1;
                }
            }
        } else {
            if (x>0) {
                if (x>0) {
                    y = x + 1;
                } else {
                    y = x + 1;
                }
            } else {
                if (x>0) {
                    y = x + 1;
                } else {
                    y = x + 1;
                }
            }
        }
    } else {
        if (x<=0) {
            if (x<=0) {
                if (x<=0) {
                    y = -x + 1;
                } else {
                    y = -x + 1;
                }
            } else {
                if (x<=0) {
                    y = -x + 1;
                } else {
                    y = -x + 1;
                }
            }
        } else {
            if (x<=0) {
                if (x<=0) {
                    y = -x + 1;
                } else {
                    y = -x + 1;
                }
            } else {
                if (x<=0) {
                    y = -x + 1;
                } else {
                    y = -x + 1;
                }
            }
        }
    }
    assert(y>0);
}