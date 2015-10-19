/*
 Le programme Tritype prend en entrée trois entiers 
 (les côtés d'un triangle) et retourne 3 si les entrées 
 correspondent à un équilatéral, 2 si elles correspondent 
 à un isocèle, 1 si elles correspondent à un autre type de 
 triangle, 4 si elles ne correspondent pas à un triangle valide.
 
 L'erreur dans ce programme est sur la condition 
 "(trityp == 1 && (i + k) > j)" (ligne 53), l'instruction 
 devra être "(trityp == 2 && (i + k) > j)". En prenant 
 comme entrée {i=2,j=2,k=2}, le programme renvoie 2 pour 
 indiquer que le triangle est un isocèle, alors que qu'il 
 devait renvoyer la valeur 4 pour signaler que l'entrée ne 
 correspond pas à un triangle valide.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

//extern void assert(int x);

/* program for triangle classification 
 * returns 1 if (i,j,k) are the sides of any triangle
 * returns 2 if (i,j,k) are the sides of an isosceles triangle
 * returns 3 if (i,j,k) are the sides of an equilateral triangle
 * returns 4 if (i,j,k) are not the sides of a triangle
 * 
 * an error has been inserted in the condition line 53 
 * when (i,j,k) = (2,2,4) returns 2 while it would return
 *    4 since the triangular inequality 2+2>4 is not verified
 */
void foo (int i, int j, int k) {//__CPROVER_assume((i == 2) && (j == 2) && (k ==4));
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
            trityp = trityp + 2;
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
                    // error in the condition : trityp == 1 instead of trityp == 2
                    if (trityp == 1 && (i+k) > j) {
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
