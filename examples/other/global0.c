
int gv1Int;
int gv2Int;
int gvArray[4];


void bar() {
    gv1Int = 0;
}

int main(int b) {

    int x;
    
    gv1Int = 0;
    gv2Int = 20;
    
    
    /*if (b) {
        gv1Int = 10;
    } else {
        gv2Int = 20;
    }*/
    x = gv1Int + gv2Int;
     
    assert(x==30);
    
    
    /*if (b) {
        gvArray[0] = 10;
    } else {
        gvArray[0] = 20;
    }
     x = gvArray[0] + gvArray[0];*/
    
    return x;
}

