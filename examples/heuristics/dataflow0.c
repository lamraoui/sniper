
// http://www.bien-programmer.fr/bits.htm

void main(int a, int b) { 
//void main() { int a, b;

   
    
    assert(a==5);
    assert(b==1);
    
    int x1 = 1 & a; // if right_bit(a)==1 then n=1
    a = a >> 1; // remove right bit
    int x2 = 1 & a; 
    a = a >> 1;
    int x3 = 1 & a;
    a = a >> 1;
    int x4 = 1 & a;
    a = a >> 1;
    
    int y1 = 1 & b; // if a==1 then n=1
    b = b >> 1; // remove right bit
    int y2 = 1 & b; 
    b = b >> 1;
    int y3 = 1 & b;
    b = b >> 1;
    int y4 = 1 & b;
    b = b >> 1;
    
    int n1 = x1 + x2 + x3 + x4;
    int n2 = y1 + y2 + y3 + y4;
    //printf("nb = %d\n", n3);
    //printf("a = %d , b = %d \n", a, b);
    
    int ret;
    if (n1>n2) {
        ret = a;
    } else {
        ret = b
    }

    
    assert(a>b);
    assert(ret==a);
}