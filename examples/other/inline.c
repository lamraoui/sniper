

// -> BUG
int foo(int x) {
    
    int y = x + 1;    
    if (y=-1) {
        y = 1;
    } else {
        y = -1;
    }
    
    
    assert(y>1);
    return y;
}


/*void foo(int x) {
    
    
    int a[0];
    a[0] = x;
    int y = a[0]; 
    
    if (y>0) {
        y = 1;
    } else {
        y = 0;
    }
    
    assert(y>1);
   // return y;
    exit(0);
}*/

/*typedef struct cell {
 int i;
 int j[];
 } cell_t;
 
 
 int foo(cell_t *c) {
 
 
 int x = c->j[3]; 
 
 return x;   
 }*/

/*int bar2(int x) {
    int y = x + 1;
    return y;
}

int bar1(int x) {
    int y = bar2(x);
    return y;
}

int bar3(int x1, int x2) {
    return x1+x2;
}

int foo(int x) {
    
    int z = x - 1;
    int y = bar3(x, z); 
    if (y>0) {
        y = 1;
    } else {
        y = 0;
    }
    
    assert(y>=1);
    return y;
}*/