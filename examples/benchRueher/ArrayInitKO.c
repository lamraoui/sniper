/*
 Le programme ArrayInit prend en entrée un tableau des entiers 
 dont les élements sont initialisés à 0. Il permet d'affecter 
 à chacune des ses cases la valeur de son indice plus 1. 
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

void ArrayInitKO(){
    int i;int a[10];
    i=0;
    while(i<10){
        a[i]=0;  // error in the assignment : should be "a[i]=i+1"
        i=i+1;
    }
    assert(a[8] != 0);
}

   
   
   
   
