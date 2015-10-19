/*
 Le programme AbsMinus prend en entrée deux entiers 
 i et j et renvoie la valeur absolue de i-j.
 
 L'erreur dans ce programme est sur l'instruction 
 d'affectation "result=i+1" (ligne 11), l'instruction 
 correcte devra être "result=i". En prenant le contre-exemple 
 {i=0,j=1}, le programme échoue et renvoie la valeur 0, alors 
 qu'il devait renvoyer la valeur 1 comme valeur absolue de i-j. 
 Toutes les conditions dans ce programme sont correctes.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

extern void assert(int x);

/* returns |i-j|, the absolute value of i minus j */
int foo (int i, int j) {//__CPROVER_assume((i==0) && (j==1));
    int result=i+1; // error in the assignment : result = i instead of result = i+1
    int k = 0;
    if (i <= j) {
        k = k+1;
    }
    if (k == 1 && i != j) {
        result = j-result; 		
    }
    else {
        result = result-j;
    }
    //assert(result==1);
    assert( (i<j && result==j-i) || (i>=j && result==i-j));
}
