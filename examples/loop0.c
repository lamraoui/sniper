
int foo(int x) {
    //assume(x>0);
	int i;
    for(i=0; i<5; i++) { 
        x = x * 2;
    
    }  
    assert(x==8);
	return 0;
}

int foo2(int x, int y) {
    assume(x>0 && y>0);
	int i;
    for(i=0; i<y; i++) { 
        x = x * 2;
    } 
    
    assert(x==8);
	return 0;
}

int foo3(int x) {
    assume(x>0);
    int a[2];
    //a[0] = x;
    //a[1] = x;
    int i;
    for(i=0; i<2; i++) { 
        a[i] = x;
    } 
    int y = 0;
    for(i=0; i<2; i++) { 
        y = y + a[i];
    }    
    assert(y==8);
	return 0;
}

int foo3b(int x) {
    assume(x>0);
    int a[2];
    int i = 0; 
    a[i] = x;
    int y = 0;
    i = 0;
    y = y + a[i];
    assert(y==8);
	return 0;
}

int add(int x) {
    int y = x + 1;
    return y;
}

int foo4(int x) {
    int y;/* uninitialized variable */
    if(x > 0)
        add(y);
    assert(y == 1);
    return y;
}

/* function foo doubles the input parameter x */
int foo5(int x) {
	/* if x == 11 * 10000 * 10000 (assume int type is 32 bit), then the assertion will fail */
    int y = x + x;
    
    int judge = 0;
    judge += (x == 0) && (y == 0);
    judge += (x > 0) && (y > 0);
    judge += (x < 0) && (y < 0);
    
	assert(judge == 1);
	return y;
}


int foo6(int x, int y) {
    
    int i = 0;
    if(x>0 && x<10) {
        while (i<x) {
            i++;
        }
    }
    assert(i>5 || i==0);
    return i;
}

int foo7(int x) {
    
    int i = 42;
    if (x>1) { // --------------------- 1
        if (x>2) { // ----------------- 2
            if (x>3) { // ------------- 3
                if (x>4) { // --------- 4
                    if (x>5) { // ----- 5
                        if (x>6) { // - 6
                            i = 42;
                        } else { // x==6
                            i = 100; // BUG
                        }
                    }   
                }   
            }   
        } 
    }
    assert(i==42);
}



/*
int binsearch(int x) {
     int a[16];
     int i;
     for (i=0; i<16; ++i) {
         a[i] = 1;
     }
     int low=0; 
     int high=16;
     int val = 0;
     while(low<high) {
         int middle=low+((high-low)>>1);
         if(a[middle]<x)
             high = middle;
         else if(a[middle]>x)
             low = middle + 1;
         else { // a[middle]=x !
             val = middle;
             break;
         }
     }
     assert(val==0 || val==1);
     return val;
}*/

/*
int a[N];
void main(){
    int i;
    a={N-1,...,0};
    BubbleSort();
    for(i=0;i<N;i++)
        assert(a[i]==i);
}
void BubbleSort(){
    int i,j,t;
    for (j=0;j<N-1;j++){
        for (i = 0; i< N-j-1; i++){
            if (a[i]>a[i+1]){
                t = a[i];
                a[i] = a[i+1];
                a[i+1] = t;
            }
        }
    }
    
    int a[N];
    void main(){
        int i;
        a={N-1,...,0};
        SelectSort();
        for(i=0;i<N;i++)
            assert(a[i]==i);
    }
    void SelectSort(){
        int i,j,t,min;
        for (j=0;j<N-1;j++){
            min=j;
            for (i=j+1; i<N; i++)
                if (a[i]>a[min])
                    min = i;
            t = a[j];
            a[j] = a[min];
            a[min] = t;
        }
    }
    
    
    int INFINITY = 899;
    void main(){
        int nodecount = 5;
        int edgecount = 10;
        int source = 0;
        int Source[10] = {0,0,1,0,3,3,0,1,1,3};
        int Dest[10] = {1,1,1,1,2,4,4,2,3,3};
        int Weight[10] = {0,1,2,3,4,5,6,7,8,9};
        int distance[5];
        int x,y,i,j;
        for(i = 0; i < nodecount; i++){
            if(i == source) distance[i] = 0;
            else distance[i] = INFINITY;
            for(i = 0; i < nodecount; i++){
                for(j = 0; j < edgecount; j++){
                    x = Dest[j];
                    y = Source[j];
                    if(distance[x] > distance[y] + Weight[j])
                        distance[x] = distance[y] + Weight[j];
                }
            }
            for(i = 0; i < edgecount; i++){
                x = Dest[i];
                y = Source[i];
                if(distance[x] > distance[y] + Weight[i]) return;
            }
            for(i = 0; i < nodecount; i++) assert(distance[i]>=0);
        }

*/