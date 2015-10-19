#include <stdlib.h>

typedef struct foo{
   
   int x;
   char * y;
   int z;

} foo_t;


foo_t * array;


int main(int n){
   int alen;  
   int * x;

   ASSUME( n > 0);
   array = (foo_t *) malloc(n * sizeof(foo_t));
   memset(array,0, sizeof(foo_t)* n);
   /*-- check
     Length(array) * sizeof(*array) >= sizeof(foo_t) * n
     
     --*/
   
   return 1;
}
