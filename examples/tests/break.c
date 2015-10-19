
int main(int x) {

    
    assert(x>=1 && x<=3);
    
    int y = 0;
    switch(x) {
        case 1:
            y = y + 0;
        case 2:
            y = y + 1;
        case 3:
            y = y + 2;
    }
    
    assert(y==0 || y==1 || y==2);
	
    return 0;
}

