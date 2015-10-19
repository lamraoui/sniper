/*
 Le programme Tritype prend en entrée trois entiers 
 (les côtés d'un triangle) et retourne 3 si les entrées 
 correspondent à un équilatéral, 2 si elles correspondent 
 à un isocèle, 1 si elles correspondent à un autre type de 
 triangle, 4 si elles ne correspondent pas à un triangle valide.
 
 L'erreur dans ce programme est sur l'instruction d'affectation 
 de la ligne 31: "trityp = trityp+1", l'instruction correcte 
 devra être "trityp = trityp + 2". En prenant comme contre-exemple 
 l'entrée {i=1,j=2,k=1}, le programme renvoie la valeur 2 
 (l'entrée correspond à un isocèle), alors qu'il devait renvoyer 
 la valeur 4 (l'entrée ne correspond pas à un triangle valide).
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

//extern void assert(int x);

/* program for triangle classification 
 * returns 1 if (i,j,k) are the sides of any triangle
 * returns 2 if (i,j,k) are the sides of an isosceles triangle
 * returns 3 if (i,j,k) are the sides of an equilateral triangle
 * returns 4 if (i,j,k) are not the sides of a triangle
 * 
 
 * an error has been inserted line 31 in the assignment
 * when (i,j,k) = (1,2,1) returns 2 while it would return 4 (not a triangle)
 */
void foo (int i, int j, int k) {//__CPROVER_assume((i == 1) && (j == 2) && (k ==1));
    int trityp;
    if (i == 0 || j == 0 || k == 0) {
        trityp = 4;		
    }
    else {
        trityp = 0;
        if (i == j) {
            trityp = trityp + 1;
        }
        if (i == k) {
            /* in the assignment : trityp = trityp + 1 instead of trityp = trityp + 2 */
            trityp = trityp + 1;   
        }
        if (j == k) {
            trityp = trityp + 3;
        }
        if (trityp == 0) {
            if ((i+j) <= k || (j+k) <= i || (i+k) <= j) {
                trityp = 4;				
            }
            else {
                trityp = 1;
            }			
        }
        else {
            if (trityp > 3) {
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
    
    assert(trityp== ((i == 0 || j == 0 || k == 0)?4:(  (i!=j && i!=k && j!=k)?(   ((i+j)<=k || (j+k)<=i || (i+k)<=j)?4:1   ):( ((i==j && j==k) || (j==k && i==k))?3:( (i==j && i!=k && j!=k && (i+j)>k)?2:( (i!=j && j!=k && i==k && (i+k)>j)?2:( ( ((i!=j && j==k && i!=k) || (i==j && j!=k && i==k)) && (j+k)>i)?2:4 ) ) ) ) )) );
}
