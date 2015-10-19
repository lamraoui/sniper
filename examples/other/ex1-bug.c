

int main(int x, int y) {
    
    assert(y>x);
    
    int r = x;
    
	if(y>x) {
		r = x;
    }
    
	assert(r>=x && r>=y);

    return r;
}
