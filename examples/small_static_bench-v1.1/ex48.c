#include <stdlib.h>
#include <string.h>

// get a sub string between two indexes.
char *str_sub(char *str, int start, int end)
{
   char *res;
   int len = strlen(str);

   res = (char *) calloc (end - start + 2, 1);
   memcpy(res, str+start, end-start+1);

   return res;
}

// remove the white space of head or tail or both.
char *str_ws_remove(char *str, int mode)
{
   if (!str) return NULL;
   
   int start = 0;
   int end = strlen(str);
   int ws_start = start, ws_end = end, i;

   for (i = 0; i < end; i++)
      if (str[i] != ' ') {
         ws_start = i;
         break;
      }

   for (i = end - 1; i >= start; i--)
      if (str[i] != ' ') {
         ws_end = i;
         break;
      }
   
   switch (mode) {
      case 0:
         start = ws_start;
         end = ws_end;
         break;
      case 1:
         start = ws_start;
         break;
      case 2:
         end = ws_end;
         break;
      default:
         ;
   }

   return str_sub(str, start, end);

}

int main(char *str){

   char *res;
   
   ASSUME(strlen(str) > 0);
   
   res = str_ws_remove(str, 0);
   free(res);
   
   return 1;
}
