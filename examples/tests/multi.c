
int main(int x) {

    
    assert(x>=1);
    assert(x<=4);
    
    int y;
    
    if(x==1)
        y = 100;
    else if(x==2)
        y = 200;
    else if(x==3)
        y = 3000; // ERROR
    else if(x==4)
        y = 400;
    else 
        y = 0;
    
    //assert(y==0 || y==100 || y==200 || y==300 || y==400);
    assert(y>=0);
    assert(y<=400);
    
    return 0;
}

