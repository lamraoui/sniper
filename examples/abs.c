int absValue(int x) {
	int abs;
    if(x>=0) {
		abs = x;
	} else {
		abs = x * 1;  // must be x*-1
	}
	assert(abs>=0);
	return abs;
}