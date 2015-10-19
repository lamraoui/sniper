int main( int n){
   int i, sum=0;
   ASSUME( n >= 0);

   for (i=0; i < n; ++i)
      sum = sum +i;

   ASSERT(sum >= 0);
}
