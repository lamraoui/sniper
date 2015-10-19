/*
 Le programme AbsMinus prend en entrée deux entiers 
 i et j et renvoie la valeur absolue de i-j.
 
 L'erreur dans ce programme est sur l'instruction 
 d'affectation "k = k+2" (ligne 14), l'instruction 
 correcte devra être "k = k+1". En prenant l'entrée 
 {i=0,j=1}, cette erreur permet de contredire la condition 
 "(k == 1 && i != j)" et donc l'exécution de l'instruction 
 "result = i-j" (ligne 20), ce qui cause une sortie autre 
 que celle attendue, -1 au lieu de 1.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */
	
/* returns |i-j|, the absolute value of i minus j */
int AbsMinusKO3 (int i, int j) {//__CPROVER_assume((i==0) && (j==1));
    int result;
    int k = 0;
    if (i <= j) {
        k = k+2; // error in the assignment : k = k+2 instead of k = k+1
    }
    if (k == 1 && i != j) {
        result = j-i; 		
    }
    else {
        result = i-j;
    }
    assert(result==1);
}
