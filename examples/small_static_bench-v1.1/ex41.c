extern int __NONDET__();
  
extern char x[101], y[101], z[201];
int from,to,i,j,k;

void mainFunc() {  
  from = __NONDET__();
  to = __NONDET__();
  k = __NONDET__();

  i = from;
  j = 0;
  while(x[i] != 0 && i < to){
    z[j] = x[i];
    i++;
    j++;
  }
  
  if(k >= 0 && k < j)
    if(z[k] == 0)
      {ERROR: goto ERROR;}  /* prove strlen(z) == j */
}
