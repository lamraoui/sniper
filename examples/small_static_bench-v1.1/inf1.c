extern int assert(int);

int foo(int a, int b, int c){
	int flag1,flag2;
	
	if (a > b) {
		flag1 = 1;
	} else {
		flag1 =0;
	}

	if (b > c){
		flag2 =1;
	} else {
		flag2 = 0;
	}

	if (flag1 == 1){
		if (flag2 == 1){
			assert( a >= c);
		}
	}

	if ( flag2 -flag1 <= 0 ) {
		if (flag1 + flag2 < 1){
			assert(a < c);
		}
	}
	return 1;
}

