#include <stdlib.h>


// Test Phi-instructions
int bar1j(int input1) {
    
    int x = input1;
    int y = 0;
    
    if (x>10) {
        y = x + 1;
    } else {
        y = 1;
    }
    int z = 0;
    if (y<10) {
        z = 1;
    } else {
        z = 2;
    }
    return z;
}


// Black-box function call
// (void return type)
void bb(int x) {  }
int bar1i(int input1) {
    
    bb(42);
    int z = 99;
    int x = 1;
    x = x + z;
    x = x + input1;
    
    int y;
    if (x>10) {
        y = 1;
    } else {
        y = 2;
    }
    return y;
}


int bar1h(int input1) {
 
    int a;
    int b;
    int c;
    int d;
    int var = 42;
    
    // S = S op S
    a = input1 + input1;
    
    // S = S op v
    b = input1 + var;
    
    // S = v op S
    c = var - input1;
    
    // v = v op v
    d = var + var;
    
    if (a==2) {
        return 1;
    }
    if (b==50) {
        return 2;
    }
    if (c==10) {
        return 3;
    }
    if (d==84) {
        return 4;
    }
    return 0;
}


// Non-linear
int bar1g(int input1) {
    int x = input1 + 1;
    x = x * x;
    
    int y = 0;
    if (x==2) {
        y = 1;
    }
    return y;
}

// Non-linear
int bar1f(int input1) {
 
    int x;
    x = (input1+1) * input1;
    
    int y = 0;
    if (x>0) {
        y = 1;
    } else {
        y = 2;
    }

    return y;
}

// Black-box function call
int bar1e(int input1) {
   
    int x = abs(input1);
    int z = 99;
    x = x + z;
    x = x + input1;
    
    int y;
    if (x>10) {
        y = 1;
    } else {
        y = 2;
    }
    return y;
}


int bar3e(int input1, int input2, int input3) {
 
    int y = 0;
    if (input1==1) {
        y = 1;
    }
    if (input2==2) {
        y = 2;
    }
    int x = input3 + input1;
    if (x==3) {
        y = 3;
    }
    return y;
}



int bar1d(int input1) {
    
    int x;
    int y = 0;
    x = input1; //bar1e(input1);
    
    if (x>10) {
        if (x==20) {
            y = 1;
        } else {
            y = 2;
        }
    } 
    else {
        y = 3;
    }
    return y;
}



int bar1a(int input1) {
    
    int x;
    int y = 0;
    x = input1;
    
    if (x==10) {
        y = 1;
    } 
    else if (x==20) {
        y = 2;  
    }
    return y;
}



int bar1c(int input1) {
    
    int x;
    int y = 0;
    
    x = input1;
    
    if (x>10) {
        if (x>20) {
            x = x + 1;
            if (x==21) {
                y = 1;
            } else {
                y = 2;
            }
        } else {
            if (x==19) {
                y = 3;
            } else {
                y = 4;
            }
        }
    } else {
        if (x==4) {
            y = 5;
        } else {
            y = 6;
        }
    }
    return y;
}



int bar1b(int input1) {
    
    int x;
    int y = 0;
    
    x = input1 + 1;
    x = x + x;
    
    if (x>10) {
        if (x==20) {
            y = 1; 
        } else {
            y = 2; 
        }
    } else {
        if (x==4) {
            y = 3;   
        } else {
            y = 4;  
        }

    }
    return y;
}


int bar3(int input1, int input2, int input3) {
    
    int x;
    int y = 0;
    
    x = input1 + input2 + input3 + 1;
    x = x + x;
    
    if (x>10) {
        y = 0;
    } else {
        y = 1;
    }
    return y;
}


int bar0() {
    
    int x;
    int y = 0;
    
    x = 1;
    x = x + x;
    
    if (x>10) {
        y = 0;
    } else {
        y = 1;
    }
    return y;
}