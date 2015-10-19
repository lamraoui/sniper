// Foo2

void main(int x, int y) {
   
    assert(x==1);
    assert(y==-1);
    
    int ret;
   if (y<0) {
       ret = 0;
   } else {
       ret = x+1;
   }
    assert(ret==1);
}