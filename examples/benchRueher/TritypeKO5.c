/*
 Le programme Tritype prend en entrée trois entiers 
 (les côtés d'un triangle) et retourne 3 si les entrées 
 correspondent à un équilatéral, 2 si elles correspondent 
 à un isocèle, 1 si elles correspondent à un autre type de 
 triangle, 4 si elles ne correspondent pas à un triangle valide.
 
 Les erreurs dans ce programme sont sur la condition "(j != k)" 
 (ligne 32) et sur la condition "(trityp >= 3)" (ligne 45), 
 les instructions correctes devront être respectivement "(j == k)" 
 et "(trityp >= 3)". En prenant l'entrée {i=2,j=3,k=3}, le 
 programme renvoie la valeur 1 (le triangle est un équilatéral), 
 alors qu'il devait renvoyer la valeur 2 (le triangle est un isocèle).
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

/* program for triangle classification 
 * returns 1 if (i,j,k) are the sides of any triangle
 * returns 2 if (i,j,k) are the sides of an isosceles triangle
 * returns 3 if (i,j,k) are the sides of an equilateral triangle
 * returns 4 if (i,j,k) are not the sides of a triangle
 * 
 
 * error has been inserted line 44, 48
 * when (i,j,k) = (2,3,3) returns 1 (any triangle) while it would return 2 (an isosceles triangle)
 */
void TritypeKO5 (int i, int j, int k) {//__CPROVER_assume((i ==2) && (j == 3) && (k ==3));
    int trityp = 0;
    if (i == 0 || j == 0 || k == 0) {
        trityp = 4;		
    }
    else {
        trityp = 0;
        if (i == j) {
            trityp = trityp + 1;
        }
        if (i == k) {
            trityp = trityp + 2;
        }
        if (j != k) {  // error in the condition : j == k instead of j != k
            trityp = trityp + 3;
        }
        if (trityp == 0) {
            if ((i+j) <= k || ((j+k) <= i || (i+k) <= j)) {
                trityp = 4;				
            }
            else {
                trityp = 1;
            }			
        }
        else {
            // error in the condition : trityp > 3 instead of trityp >= 3
            if (trityp >= 3) { 
                trityp = 3;
            }
            else {
                if (trityp == 1 && (i+j) > k) {
                    trityp = 2;
                }
                else {
                    if (trityp == 2 && (i+k) > j) {
                        trityp = 2;						
                    }
                    else {
                        if (trityp == 3 && (j+k) > i) {
                            trityp = 2;
                        }
                        else {
                            trityp = 4;
                        }
                    }
                }
            }
        }
    }
    assert(trityp == 2);
}
