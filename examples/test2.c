int absValue(int x) {
	int abs;
	if(x>=0) {
		abs = x;
	} else {
		abs = x * 1;   // should be: abs=x*-1;
	}
	assert(abs>=0);
	return abs;
}

void foo(int x) { 
    int y = x;
    if (y > 0) {
        y = 1 - x;
    } else {
        y = 42;
    }
    assert (y > 1);
}


void foo2(int x) 
{ 
    int y;
    int z = x;
    if (z >= 1) {
        y = 2 - x;
    } else {
        y = x;// - 2;
    }
    assert (y >= 1);
}



