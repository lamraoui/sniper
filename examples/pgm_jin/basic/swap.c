/*
   pre:  x and y are both integers
   post: x == old_y
     and y == old_x
*/

/* function foo swaps the value of its two input parameters*/
void foo(int x, int y) {
    int old_x = x, old_y = y;
    int temp;
    temp = x;    
    /* y = temp;*/ /* This is a wrong place!*/
    x = y;
    y = temp;/* if this line is moved to line 12, something will go wrong */
        
	assert(x == old_y && y == old_x);
}
