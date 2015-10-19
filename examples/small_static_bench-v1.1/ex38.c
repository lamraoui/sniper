#include <string.h>


int main(int m, int n){
   char  s1[100], s2[100],  s3[200];
   ASSUME( m > 0);
   ASSUME( m < 100);

   s1[m-1]=0;
   s2[m-1]=0;

   strcpy(s3,s1);
   strcat(s3,s2);

   ASSERT( strlen(s3) <= 200 ); 
   ASSERT(strlen(s3) <= 2 * m); 

   return 1;
}
