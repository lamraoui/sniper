/*
 Le programme AbsMinus prend en entrée deux entiers 
 i et j et renvoie la valeur absolue de i-j.
 
 L'erreur dans ce programme est sur l'instruction 
 d'affectation "result = i-j" (ligne 17), l'instruction 
 correcte devra être "result = i-j". En prenant comme 
 entrée de ce programme {i=0,j=1}, il retourne la valeur -1 
 comme valeur absolue de i-j, alors qu'il devait retourner 1. 
 Ce programme programme présente le cas où toutes les condition 
 sont correctes.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

//extern void assert(int x);

/* returns |i-j|, the absolute value of i minus j */
int foo (int i, int j) {//__CPROVER_assume((i==0) && (j==1));
    int result;
    int k = 0;
    if (i <= j) {
        k = k+1;
    }
    if (k == 1 && i != j) {
        result = i-j; // error in the assignment : result = i-j instead of result = j-i
    }
    else {
        result = i-j;
    }
    //assert(result==1);
    assert( (i<j && result==j-i) || (i>=j && result==i-j));
}
