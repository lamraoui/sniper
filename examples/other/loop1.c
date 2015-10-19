
int main(int x) {

    x = 2;
	int i;
    for(i=0; i<2; i++) { 
        x = x * 2;
        if (x==8) {
            x = 1;
        }
    } 
    
	return x;
}

