// Foo

void main(int a, int b) {
    int c, d, e ,f;
    
  
    
    if (a>=0) {
        c = 0;
    } else {
        c = b;
        d = 1;
        e = -a;
        if (a>b) {
            f = b + e + a;
            d = d + 4;
        } else {
            c = 0; 
        }
    }

   
    assert(c>=d+e);
}