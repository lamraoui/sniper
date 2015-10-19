
int main(int x) {

    x = 2;
	int i,j;
    for(i=0; i<2; i++) { 
        x = x * 2;
        for(j=0; j<2; j++) { 
            x = x + x;
        } 
    } 
    
	return x;
}

