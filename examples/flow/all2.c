int absValue2(int input) {
    int sign, abs;
    abs = input;
    if (input==0) {
        return 0;
    }
    if (input<0) {
        sign = -1;
        printf("negative");
    } else {
        sign = -1;          // BUG1
        printf("positive");
    }
    if (sign==-1) {
        sign = input * -1;  // BUG2
    } else {
        abs = input;
    }
    assert( abs>=0 && ((input<0 && sign==-1) || 
                       (input>=0 && sign!=-1)) );    
    return abs;
}


int process (int x, int y) {
    int z = 0;
    int lock = 1;
    if(x==0) {
        if (y==0) {
            z = 1;
        }
    }
    if (y!=0) {
        z = y;
    }
    if (x!=0) {
        z = 2;
        lock = 0;
    } else if (z>0) {  // BUG
        z = 3;
        lock = 0;
    }
    assert(lock==0);
}


int b2d(int a, int b, int c, int d) {
    assume((a==1 || a==0) 
           && (b==1 || b==0) 
           && (c==1 || c==0) 
           && (d==1 || d==0));
    
    int v = 0;
    if (a==1) {
        v = v + 1;
    }
    if (b==1) {
        v = v + 18; // BUG1: should be v=v+2;
    }
    if (c==1) {
        v = v + 4;
    }
    if (d==1) {
        v = v - 8; // BUG2: should be v=v+8;
    }
    assert(v==(a*1+b*2+c*4+d*8));
    return v;
}


void max(int a, int b) {
    
    int max = 0;
    if (a<b) {          // BUG1: should be (a>b)
        max = a;
    } else if (b<a) {    // BUG2: should be (b>a)
        max = b;        
    }

    assert( (max==a && a>b) ||  (max==b && b>a) ||  (max==0 && a==b));
    
}
