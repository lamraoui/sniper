/*
 Le programme TriPerimetre a exactement la même structure 
 de contrôle que tritype. La différence est que TriPerimetre 
 renvoie un calcul sur les entrée et pas une constante. 
 Le programme renvoie la somme des côtès du triangle si 
 les entrées correspondent à un triangle valide ; et 
 retourne -1 dans le cas inverse. 
 
 L'erreur dans ce programme est sur la deuxième partie de 
 la condition (trityp == 1 && (i+j) > k) (ligne 57), l'instruction 
 correcte devra être (trityp == 2 && (i+k) > j). En prenant 
 l'entrée {i=1, j=2, k=1}, le programme fournit en sortie la 
 somme 4, or il devrait retourner la valeur -1 pour signaler 
 que les entrées ne correspondent pas à un triangle valide.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

/* program for triangle perimeter 
 * returns i+j+k
 *      i+j+k if (i,j,k) are the sides of any triangle
 *      2*i + j or 2*i+k or 2*j+i if (i,j,k) are the sides of an isosceles triangle
 *      3*i if (i,j,k) are the sides of an equilateral triangle
 * returns -1 if (i,j,k) are not the sides of a triangle
 * 
 * ERROR : in assignment line 34
 * returns 9 for (2,3,2) while it should return 7
 */
void TriPerimetreKOV2 (int i, int j, int k) {//__CPROVER_assume((i == 2) && (j == 3) && (k ==2));
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
            res = 2*i;
        }
        if (i == k) {
            trityp = trityp + 2;
            res = 2*j;  // error in the assignment : should be res = 2*i
        }
        if (j == k) {
            trityp = trityp + 3;
            res = 2*j;
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
                res = res + i;
            }
            else {
                if (trityp == 1 && (i+j) > k) { // i==j
                    res=res+k; 
                }
                else {						
                    if (trityp == 2 && (i+k) > j) {    // i==k
                        res = res + j;						
                    }
                    else {
                        if (trityp == 3 && (j+k) > i) {   // j==k
                            res=res+i;
                        }
                        else {
                            res=-1;
                        }
                    }
                }
            }
        }
    }
    assert(res == 7);
}
