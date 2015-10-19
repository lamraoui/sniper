/*
 Le programme TriPerimetre a exactement la même structure 
 de contrôle que tritype. La différence est que TriPerimetre 
 renvoie un calcul sur les entrée et pas une constante. 
 Le programme renvoie la somme des côtès du triangle si 
 les entrées correspondent à un triangle valide ; et 
 retourne -1 dans le cas inverse. 
 
 L'erreur dans ce programme est sur l'instruction d'affectation 
 "res = 2*j" (ligne 34), l'instruction correcte devra être 
 "res = 2*i". En prenant comme entrée {i=2,j=3,k=2}, le programme 
 renvoie que la somme du triangle est 9, alors qu'il devait renvoyer 
 la somme 7. Toutes les conditions présentes dans ce programme 
 sont correctes. Ce programme est sémantiquement équivalent à 
 TriPerimetreKO, la différence est que l'erreur provoquée est 
 n'est pas sur la même instruction pour les deux programmes.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

extern void assert(int x);

/* program for triangle perimeter 
 * returns i+j+k
 *      i+j+k if (i,j,k) are the sides of any triangle
 *      2*i + j or 2*i+k or 2*j+i if (i,j,k) are the sides of an isosceles triangle
 *      3*i if (i,j,k) are the sides of an equilateral triangle
 * returns -1 if (i,j,k) are not the sides of a triangle
 * 
 * ERROR in condition line 57 : returns 3 for (1,1,2) while it should return -1
 */
void foo (int i, int j, int k) {//__CPROVER_assume((i == 1) && (j == 1) && (k == 2));
    int trityp = 0;
    int res;
    if (i == 0 || j == 0 || k == 0) {
        trityp = 4;		
        res = -1;
    }
    else {
        trityp = 0;
        if (i == j) {
            trityp = trityp + 1;
        }
        if (i == k) {
            trityp = trityp + 2;
        }
        if (j == k) {
            trityp = trityp + 3;
        }
        if (trityp == 0) {
            if ((i+j) <= k || ((j+k) <= i || (i+k) <= j)) {
                trityp = 4;
                res = -1;
            }
            else {
                trityp = 1;
                res = i+j+k;
            }			
        }
        else {
            if (trityp > 3) {
                res = 3*i;
            }
            else {
                if (trityp == 1 && (i+j) > k) { // i == j
                    res=2*i+k;
                }
                else {
                    // error in the condition : should be trityp == 2
                    if (trityp == 1 && (i+k) > j) {  // i==k
                        res = 2*i + j;						
                    }
                    else {
                        if (trityp == 3 && (j+k) > i) {  // j == k
                            res=2*j+i;
                        }
                        else {
                            res=-1;
                        }
                    }
                }
            }
        }
    }
    //assert(res == -1);
    
    assert(res== ((i==0 || j==0 || k==0)?-1:(  (i!=j && i!=k && j!=k)?(   ((i+j)<=k || (j+k)<=i || (i+k)<=j)?-1:(i+j+k)   ):( ((i==j && j==k) || (j==k && i==k))?(3*i):( (i==j && i!=k && j!=k && (i+j)>k)?(2*i+k):( (i!=j && j!=k && i==k && (i+k)>j)?(2*i+j):( ( ((i!=j && j==k && i!=k) || (i==j && j!=k && i==k)) && (j+k)>i)?(2*j+i):-1 ) ) ) ) )) );
    
}
