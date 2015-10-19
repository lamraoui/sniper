#include <stdlib.h>

#define INIT_SIZE 100

typedef struct _vector
{
	size_t size;
	size_t capacity;
	int *data;
} Vector;

Vector* create_vector()
{
	Vector *node;

	node = (Vector *) malloc(1 * sizeof(Vector));
	node->size = 0;
	node->capacity = INIT_SIZE;
	node->data = (int *)malloc(node->capacity* sizeof(int));
        ASSUME(node->data);
	return node;
}

void push_back(Vector *vct, int value)
{
	int *tmp;
        ASSERT(vct -> size <= vct -> capacity);
	if (vct->size >= vct->capacity)
	{
		vct->capacity += INIT_SIZE;
		tmp = (int*) malloc (vct->capacity * sizeof(int));
                ASSUME(tmp != NULL);
		memcpy(tmp, vct->data, (vct->capacity - INIT_SIZE)*sizeof(int));
		free(vct->data);
		vct->data = tmp;
	}
        
	vct->data[vct->size++] = value;
}

int main(int n){
   int i;
   Vector * a;
   ASSUME(n > 0);
   
   a = creat_vector();
   
   for (i =0; i<n; i++) 
   {
       push_back(a, i);
   }
   
   ASSERT(a->size == n);

   return 1;
}
