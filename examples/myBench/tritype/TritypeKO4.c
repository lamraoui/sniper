/*
 Le programme Tritype prend en entrée trois entiers 
 (les côtés d'un triangle) et retourne 3 si les entrées 
 correspondent à un équilatéral, 2 si elles correspondent 
 à un isocèle, 1 si elles correspondent à un autre type de 
 triangle, 4 si elles ne correspondent pas à un triangle valide.
 
 L'erreur dans ce programme est sur l'instruction "(trityp >= 3)", 
 l'instruction devra être "(trityp > 3)". En prenant l'entrée 
 suivante {i=2,j=3,k=3}, le programme satisfait la condition et 
 exécute l'instruction "trityp = 3" (ligne 46) pour renvoyer que 
 le triangle en entrée est un équilatéral, alors qu'il devrait 
 la contredire et renvoyer que qu'il est isocèle (la valeur 
 renvoyée devait être égale à 2).
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

extern void assert(int x);

/* program for triangle classification 
 * returns 1 if (i,j,k) are the sides of any triangle
 * returns 2 if (i,j,k) are the sides of an isosceles triangle
 * returns 3 if (i,j,k) are the sides of an equilateral triangle
 * returns 4 if (i,j,k) are not the sides of a triangle
 
 * an error has been inserted in the condition line 45
 * when (i,j,k) = (2,3,3) returns 3 while it would return
 *    2 (an isosceles triangle)
 */
void foo (int i, int j, int k) {//__CPROVER_assume((i == 2) && (j == 3) && (k ==3));
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
        if (j == k) {
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
    //assert(trityp == 2);
    
    assert(trityp== ((i == 0 || j == 0 || k == 0)?4:(  (i!=j && i!=k && j!=k)?(   ((i+j)<=k || (j+k)<=i || (i+k)<=j)?4:1   ):( ((i==j && j==k) || (j==k && i==k))?3:( (i==j && i!=k && j!=k && (i+j)>k)?2:( (i!=j && j!=k && i==k && (i+k)>j)?2:( ( ((i!=j && j==k && i!=k) || (i==j && j!=k && i==k)) && (j+k)>i)?2:4 ) ) ) ) )) );
}
