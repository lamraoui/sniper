extern int __NONDET__();
  
char x[101], y[101], z[201];
int from,to,i,j,k;

void main(){
  from = __NONDET__();
  to = __NONDET__();
  k = __NONDET__();

  if(!(k >=0 && k <= 100 && x[k] == 0))  /* assume strlen(x) <= 100 */
  {stuck1: goto stuck1;}

  if(!(from >= 0 && from <= k))            /* assume "from" index is O.K. */
  {stuck1: goto stuck1;}
  
  /* extract substring form index "from" to index "to" */
  
  i = from;
  j = 0;
  while(x[i] != 0 && i < to){
     z[j] = x[i];
     i++;
     j++;
  }
  
  z[j] = 0;
  
  if(!(j <= 100))
  {ERROR: goto ERROR;}  /* prove we don't overflow z */
}
