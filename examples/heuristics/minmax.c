// Minmax

/*
 
 BugAssist:
< 1 > Potential BUG @ :./ex/minmax.c:24 : least = c;
< 2 > Potential BUG @ :./ex/minmax.c:21 : most = b; //BUG, should be  least = b;
< 3 > Potential BUG @ :./ex/minmax.c:20 : if (least>b) {
*/
 
void minmax(int a, int b, int c) {
    
    // counter example 1
    //assert(a==1); assert(b==0); assert(c==1);
    // counter example 1
    //assert(a==-1610612736); assert(b==-2147483647); assert(c==-1610612735);
    
    
    int least = a;
    int most = a;
    if (most<b) {
        most = b;
    }
    if (most<c) {
        most = c;
    }
    if (least>b) {
        most = b; //BUG least = b;
    }
    if (least>c) {
        least = c;
    }
    
    assert(least<=most);
}