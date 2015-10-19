
int foo(int index) {

    
    assert(index==1);
    
    
	if(index!=1) 
		index = 2;
	else
		index = index + 2;
    
	int i = index;
    
    assert(i>=0 && i<3);
	return 0;
}

