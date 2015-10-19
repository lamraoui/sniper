
int foo1(int x) {
    int y = 0;
	switch (x) {
        default:
            y = 1;
            break;
    }
    assert(y==0);
	return y;
}

int foo2(int x) {
    int y = 0;
	switch (x) {
        case 1:
            y = 2;
            break;
        default:
            y = 1;
            break;
    }
    assert(y==0);
	return y;
}

int foo3(int x) {
    int y = 0;
	switch (x) {
        case 1:
            y = y + 1;
            y = y + 1;
            break;
        case 2:
            y = y + 2;
            y = y + 2;
            break;
        case 3:
            y = y + 3;
            y = y + 3;
            break;
        default:
            y = 0;
            break;
    }
    assert(y==10);
	return x;
}

