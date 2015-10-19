
int main(int arg) {
    
    int a[4];
    int b[2];
    int x; 
    
    if (arg==1) {
        a[0] = 100;
        b[0] = 200;
    } else {
        a[0] = 300;
        b[0] = 400;
    }

    x = a[0] + b[0];
    
    return x;
}