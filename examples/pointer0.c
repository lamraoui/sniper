
/*struct RT {
  char A;
  int B[10][20];
  char C;
};
struct ST {
  int X;
  double Y;
  struct RT Z;
};

int main() {

struct ST s[10];

  return s[1].Z.B[5][13];
}*/


/*int main() {

    int x;
    x = 10;
    int *v;
    v = &x;
    *v = 0;
    assert(x>0);
    return x;
}*/


/*void foo(int *v, int c);
int main() {

    int y = 9;
    int x;
    foo(&x, y);

    assert(x>30);
	return x; 
}
void foo(int *v, int c) {
    int z = 3;
    if (c>z) {
        *v = 10;
    } else {
        *v = 20;
    }
}*/



/*int main() {

    int a[10][10];

	return a[1][3]; 
}*/

int main(int x) {

    int *p;
    int a[10];
    
    p = &a[0];
    
    p++;
    *p = 10;
    p++;
    *p = 20;
    p = p + 3;
    *p = 30;
    p = p - 5;
    *p = 11; 
    
	return a[1]; 
}


/*int main(int x) {

    int a, b, c, d;
    int *p;
    int T[10];
    
    p = &T[0];
    
    p++;
    a = *p;
    p++;
    b = *p;
    p = p + 3;
    c = *p;
    p = p - 5;
    d = *p; 
    
	return a+b+c+d; 
}*/



/*int main(int x, int y) {
    int *p;
    int a[10];
    p = &a[x];
    p[1] = 42;
	return a[x+1]; 
}
*/



/*
int main(int x, int y) {
    int *p1, *p2;
    int r;
    p1 = &x;
    p2 = &y;
    r = *p1 + *p2;
    return r;
}
*/