
int main() {

    int x; 
    int y;
    
    assert(x>0);
    
    
	if(x==0) 
		x = 1;
	
    
    if(x>1) 
		x = 100;
    
    
	y = x - 1;
    
    assert(y==9);
	return 0;
}

