
int gvArray[4];

int main(int arg) {
    
    //int x;
    //int a[4];
    

    /*a[1] = 100;
    x = a[1];
    a[0] = 200;
    x = a[0];
    a[0] = 300;
    x = a[0];*/
    
   /* if (arg==2) {

        if(arg==1)
            a[0] = 100;
        else 
            a[0] = 200;

    } else {
        x = a[0];
    }*/
    
    
    
   /* if(arg==1)
        a[0] = 100;
    else if(arg==2) {
        a[0] = 200;
    } else {
        a[0] = 300;
    }
    //a[0] = 400;*/
    
    
    
    /*a[0] = 100;
    if (arg==1) {
        a[1] = 1;
    } else {
        a[1] = 2;
    }
    a[0] = a[0] + a[1];*/

    
    //return a[0];
    
    
    
    
    // TEST GLOBAL VARIABLES
    
    gvArray[0] = 100;
    if (gvArray[0]==100) {
         gvArray[0] = gvArray[0] + 200;
    } else {
         gvArray[0] = gvArray[0] + 300;
    }

    
    return gvArray[0];
    
}
