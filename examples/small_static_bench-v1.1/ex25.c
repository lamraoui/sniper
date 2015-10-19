int a[1000];

int foo(int x){
   int i;
   if (x == 0) return 1;
   assert(x < 1000);
   assert(x > 0);

   for (i = 0; i < x; ++i)
      a[i]=0;
   return 0;
}


int main(){
   int y;
   int z;
   for (y=0; y < 100; ++y)
      foo(y);

   for (z=0; z < 1000; ++z)
      foo(z);

   return 1;

}
