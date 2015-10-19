int b2d(char a, char b, char c, char d) {
    
    assume((a==1 || a==0) 
           && (b==1 || b==0) 
           && (c==1 || c==0) 
           && (d==1 || d==0));
    
    int v = 0;
    
    if (a==1) {
        v = v + 1;
    }
    if (b==1) {
        v = v + 2;
    }
    if (c==1) {
        v = v + 4;
    }
    if (d==1) {
        v = v - 8;
    }
    
    assert(v==(a*1+b*2+c*4+d*8));
    
    return v;
}

/*
 pre  : x > 0
 post : y > 0
 */

void test(int x) {
    
    assume(x>0);
    
    int y;
    if(x < 10)
        y = 10 - x;
    else
        y = x - 10;
    
    assert(y>0);
}


void test2(int a, int b, int c, int d) {
 
    int out = 0;
    if (a>0) {
        out++;
    }
    if (b>0) {
        out++;
    }
    if (c>0) {
        out++;
    }
    if (d>0) {
        out++;
    }
    assert(out>3);
}




int test6(int a, int b, int c , int d) {

    
    assume(a>0 && b>0);
    
    int field1 = 0;
    int field2 = 0;
	if (c>0) {
	    if (d>0) {
            field1 = a;
            field2 = b;
	    }
	    else {
            field1 = a;
	    }
	}
	else {
	    if (d>0) {
            field2 = a; // should be field2 = b;
	    }
	    else {
            field1 = -1;
            field2 = -1;
	    }
	}
	assert( ((c>0 && field1==a)  &&  (d>0 && field2==b)) || (field1=-1 && field2==-1) );


}


int trafficLight(int state) {
    
    assume(state==3);
    
	int finalState;
    if (state==0) {
        finalState = 1;
    } else {
        if (state==1) {
            finalState = 3;
            // should be finalState = 2;
        } else {
            if (state==2) {
                finalState = 3;
            } else {
                finalState = 3;
                // should be finalState = 0;
            }
        }
    }
	assert(((finalState-1)==state) || (state>2 && finalState==0));
}



