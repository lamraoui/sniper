

// single fault - nested ifs
// -max-depth 5
int foo1(int x) {
    
    int i = 42;
    if (x>1) { // --------------------- 1
        if (x>2) { // ----------------- 2
            if (x>3) { // ------------- 3
                if (x>4) { // --------- 4
                    if (x>5) { // ----- 5
                        if (x>6) { // - 6
                            i = 42;
                        } else {     // x==6
                            i = 100; // BUG
                        }
                    }   
                }   
            }   
        } 
    }
    assert(i==42);
}

// single fault - else ifs
// -max-depth 6
int foo2(int x) {
    
    int y;
    if (x==1) {
        y = 42;
    } else if (x==2) {
        y = 42;
    } else if (x==3) {
        y = 42;
    } else if (x==4) {
        y = 42;
    } else if (x==5) {
        y = 42;
    } else if (x==6) {
        y = 41; // BUG
    } else {
        y = 0;
    }
    
    assert(y==0 || y==42);
    return y;
}

// multifault - else ifs
// (first bug)            -max-depth 3
// (first and second bug) -max-depth 6
int foo3(int x) {
    
    int y;
    if (x==1) {
        y = 42;
    } else if (x==2) {
        y = 42;
    } else if (x==3) {
        y = 40; // BUG
    } else if (x==4) {
        y = 42;
    } else if (x==5) {
        y = 42;
    } else if (x==6) {
        y = 41; // BUG
    } else {
        y = 0;
    }
    
    assert(y==0 || y==42);
    return y;
}

// single fault - loop
// -max-depth 11 -unroll 5
int foo4(int x) {
    
    int i = 0;
    int y = x;
    while (i<y) {
        if (i==5) {
            x = 42; // BUG
        }
        i++;
    }    
    assert(x==i);
    return i;
}

// multifault - loop
// (first bug)            -max-depth 1 -unroll 1
// (first and second bug) -max-depth 10 -unroll 10
int foo5(int x) {
    
    int y = x;
    int div_counter = 0;
    while (y>0) {
        y = y / 2;
        div_counter++;
    }
    int res = 0;
    if (div_counter==0) {
        res = 1;
    }
    if (div_counter==10) {
        res = 2;
    }
    assert(res==0);
    return div_counter;
}


