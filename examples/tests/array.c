int main(int index) {
    
    int A[3];
    assert(index==1);
    
    
	if(index!=1) 
		index = 2;
	else
		index = index + 2;
    
	int i = index;
    
    assert(i>=0 && i<3);
    
	return A[i];
}
