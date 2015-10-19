int a[2];

int bar() {
    return 10;
}


int foo(int x) {
    
   /* if (x<10) {
        a[0] = 1;
        a[1] = 0;
    } else {
        a[0] = 0;
        a[1] = 2;
    }*/
    int globalvar;
   
    if (x<10) {
        globalvar = x + bar();
    } else {
        globalvar = x - 10;
    }

    
    assert(globalvar==1);
    
    
    return globalvar;
}