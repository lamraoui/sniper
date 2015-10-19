/********************************
 * array.c
 * 17/12/2013
 *
 * Toy programs for testing the 
 * concolic module of SNIPER.
 ********************************/

//int global;
 int a[2];

// multiple failing/successful paths
int foo(int input) {

    
    //global = 0;
    
   
    a[0] = -10;
    a[1] = 10;
    
    int out;
	if (a[input]>=0) {
        out = input-10;
    } else {
        out = input+10;
    }
    assert(out==0);
	return 0;
}