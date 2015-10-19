/*
   pre:  x is an integer
   post: x is even implies y == 1
     and x is odd implies y == 0
*/

/*	
	misuse of "=" instead of "=="
*/

/* function foo judges whether x is odd */
int foo (int x) {
	int y = 0;
	if(x = x / 2 + x / 2)/* "=" should be "==" */
		y = 1;
	else
		y = 0;
		
	int judge = 0;
	judge += (x != x / 2 * 2) && (y == 0);/* x is odd */
	judge += (x == x / 2 * 2) && (y == 1);/* x is even */
	
	assert(judge == 1);
	return y;
}
