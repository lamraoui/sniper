/*int foo(int x) {
    int y = 0;
    if(x>0) {
        y = 42;
    } else {
            for(int i=0; i<10; i++) {
                y = y + 1;
            }
    }
    assert(y<=10);
    return y;
}

int foo1(int x) {
	int y = 0;
	if(x>0) {
		y = y + 1;
	} else {
		y = y + 2;
	}
	assert(y>1);
	return y;
}

char foo2(int x) {

    char c;
    char p[10];
    p[0] = 'H';
    if (x==1) {
        c = '1';
    } else {
        c = '<'; // should be '>'
    }
    assert(c=='1' || c=='>');
    return c;
}*/

int foo3(int x) {
	int y;
	if(x>=0) {
		y = x * 2;
		y = y * 4;
	} else {
		y = 1 + x; // Should be: y=1-x;
		y = y * 8;
		y = y * 16;
	}
	assert(y>=0 && y>=x);
	return y;
}