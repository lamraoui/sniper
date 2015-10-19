
int main(int x) {

    assert(x==3);

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
            y = y + 30;
            break;
        default:
            y = 0;
            break;
    }
    
    //assert(y>=0 && y<=9);
    //assert(y>=0);
    assert(y<=9);
           
	return x;
}

