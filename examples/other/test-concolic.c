/********************************
 * concolic.c
 * 10/11/2013
 *
 * Toy programs for testing the 
 * concolic module of SNIPER.
 ********************************/


#include <math.h> // for abs()


// multiple failing/successful paths
int foo1a(int input) {

    int out;
    int x = input+1;
	if (x>=0) {
        out = input-10;
    } else {
        out = input+10;
    }
    assert(out==0);
	return 0;
}

// assert with non-symbolic variable
int foo1b(int input) {
    
    int out;
	if (input>=0) {
        out = 1;
    } else {
        out = 2;
    }
    assert(out==1);
	return 0;
}

// assert with symbolic variable
int foo1c(int input) {
    int x = 0;
    if (input==2) {
        x = input * 8;
    }
    if (input==4) {
        x = input * 8; // should be x=i*4
    }
    if (input==8) {
        x = input * 2;
    }
    assert(x==0 || x==16);
    return 0;
}

// black-box function call
// (with successful path)
int foo2a(int input) {
    
    int out;
	if (input>=0) {
        out = -1;
        out = abs(out);
    } else {
        out = -2;
        out = abs(out);
    }
    assert(out==1);
	return 0;
}




// black-box function call
// (without successful path)
int foo2b(int input) {
    
    int out;
	if (input>=0) {
        out = -2;
        out = abs(out);
    } else {
        out = -2;
        out = abs(out);
    }
    assert(out==1);
	return 0;
}

// non-linear operation
// (with successful path)
int foo3a(int input) {
    
    int x;
    int y;
    int z;
	if (input>=0) {
        y = 2;
        z = 4;
    } else {
        y = 1;
        z = 2;
    }
    x = y * z;
    assert(x>=4);
	return 0;
}

// non-linear operation
// (without successful path)
int foo3b(int input) {
    
    int x;
    int y;
    int z;
	if (input>=0) {
        y = 2;
        z = 2;
    } else {
        y = 2;
        z = 2;
    }
    x = y * z;
    assert(x==2);
	return 0;
}

// multi-values paths
int foo4a(int input) {
    int x = 0;
    if (input<=2) {
        x = input * 8;
    }
    if (input>=4) {
        x = input * 4; 
    } 
    assert(x==0 || x==16);
    return 0;
}