void main(int x) {
	//__CPROVER_assume(x > 0);
	assert(x > 0);
    int tmp = 0;
	tmp = x + 1;
	if(tmp==2) {
		tmp = tmp * 2;
	}
	assert(tmp < 0);
}
