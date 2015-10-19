/*
 Le programme TriPerimetre a exactement la même structure 
 de contrôle que tritype. La différence est que TriPerimetre 
 renvoie un calcul sur les entrée et pas une constante. 
 Le programme renvoie la somme des côtès du triangle si 
 les entrées correspondent à un triangle valide ; et 
 retourne -1 dans le cas inverse. 
 
 L'erreur dans ce programme est sur l'instruction d'affectation 
 "res = 2*i + k" (ligne 58), l'instruction devra être "res = 2*i+j". 
 En prenant l'entrée {i=2,j=1,k=2}, le programme renvoie comme 
 somme des cotés du triangle correspondant la valeur 6, alors 
 qu'il devait renvoyer plutôt la valeur 5. Toutes les conditions 
 dans les programme sont correctes.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

/* program for triangle perimeter 
 * returns i+j+k
 *      i+j+k if (i,j,k) are the sides of any triangle
 *      2*i + j or 2*i+k or 2*j+i if (i,j,k) are the sides of an isosceles triangle
 *      3*i if (i,j,k) are the sides of an equilateral triangle
 * returns -1 if (i,j,k) are not the sides of a triangle
 * 
 * ERROR : in assignment line 58
 * returns 6 for (2,1,2) while it should return 5
 */
void TriPerimetreKO (int i, int j, int k) {//__CPROVER_assume((i == 2) && (j == 1) && (k ==2));
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
                if (trityp == 1 && (i+j) > k) { // i==j
                    res=2*i+k; 
                }
                else {
                    // error in the assignment : should be res = 2*i+j
                    if (trityp == 2 && (i+k) > j) {    // i==k
                        res = 2*i + k;						
                    }
                    else {
                        if (trityp == 3 && (j+k) > i) {   // j==k
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
    assert(res == 5);
}
