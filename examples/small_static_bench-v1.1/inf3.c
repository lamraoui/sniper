
#define NULL 0

int zeroOut( int * a, int n){
  int i;
  if ( a == NULL){
    return -1;
  }

  for (i=0; i< n; ++i){
    a[i]=0;
  }
  return n;
}


int average(int * a, int n){
  int i,sum=-1;
  if ( a == NULL){
    return -1;
  }
  
  for (i=0; i< n; ++i){
    if (sum == -1){
      sum = a[i];
      ASSUME(sum > 0);
    } else {
      sum += a[i];
      ASSUME(sum > 0);
    }
  }

  if (sum == -1){
    return -1;
  }

  return sum;
}


int main(int n, int x){
  int * a;
  int tmp;
  if (n <= 0){
    if (__NONDET__()){
      a = NULL;
    } else {
      a = & x;
      n=1;
    }
  } else {
    a = (int *) malloc(n * sizeof (int));
  }

  tmp = zeroOut(a,n);
  if (average(a,n) <= 0){
    ASSERT(tmp <= 0);
  } else {
    return 0;
  }

  return 1;
}
