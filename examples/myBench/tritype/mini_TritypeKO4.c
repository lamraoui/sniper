/*
 
 number of hard constraints  105
 number of soft constraints  12
 number of error-in. inputs  1
 
 
 -- Error-inducing Input: (and (= i 2) (= j 1) (= k 1)).00 %
 
 {{57}, {32}}
 
 */
void assert(int x);

void foo (int i, int j, int k) {
            int trityp = 0;
            if (i == 0 || j == 0 || k == 0) {
                // (or (and (= _trityp1 4) bb14_bb68) (not bb14))
/*HARD*/        trityp = 4;
            }
            else {
                trityp = 0;
                if (i == j) {
                    //(or (and (= _trityp2 0) (= tmp18 1) bb17_bb19) (not bb17))
/*HARD*/            trityp = trityp + 1;
                }
                if (i == k) {
                    trityp = trityp + 2;
                }
                if (j == k) {
                    // (or (and (= trityp.0 1) (= tmp22 3) bb21_bb23) (not bb21))
/*HARD*/            trityp = trityp + 3;
                }
                if (trityp == 0) {
                    // (or (and (= i 3) (= j 2) (= k 1) (= trityp.3 4) bb40_bb67) (not bb29))
/*HARD*/           if ((i+j) <= k || ((j+k) <= i || (i+k) <= j)) {
/*HARD*/                trityp = 4;
/*HARD*/           }
/*HARD*/           else {
/*HARD*/                trityp = 1;
/*HARD*/           }
                }
                else {
                    // BUG! Must be trityp > 3
                    if (trityp >= 3) {
                        trityp = 3;
                    }
                    
    // (or (and (= trityp.2 1) (= i 2) (= j 2) (= k 1) (= trityp.6 2) bb65_bb66)
    //     (and (= trityp.2 2) (= i 2) (= k 2) (= j 1) (= trityp.6 2) bb65_bb66)
    //     (not bb44)
    // )
                    else {
/*HARD*/                if (trityp == 1 && (i+j) > k) {
/*HARD*/                    trityp = 2;
/*HARD*/                }
/*HARD*/                else {
/*HARD*/                    if (trityp == 2 && (i+k) > j) {
/*HARD*/                        trityp = 2;
/*HARD*/                    }
/*HARD*/                   else {
/*HARD*/                        if (trityp == 3 && (j+k) > i) {
/*HARD*/                            trityp = 2;
/*HARD*/                        }
/*HARD*/                        else {
/*HARD*/                            trityp = 4;
/*HARD*/                        }
                            }
                        }
                    }
                }
            }
    //assert(trityp == 2);
    //assert((trityp==2 && i==2 && j==3 && k==3) || i!=2 || j!=3 || k!=3);
    
    assert(trityp== ((i == 0 || j == 0 || k == 0)?4:(  (i!=j && i!=k && j!=k)?(   ((i+j)<=k || (j+k)<=i || (i+k)<=j)?4:1   ):( ((i==j && j==k) || (j==k && i==k))?3:( (i==j && i!=k && j!=k && (i+j)>k)?2:( (i!=j && j!=k && i==k && (i+k)>j)?2:( ( ((i!=j && j==k && i!=k) || (i==j && j!=k && i==k)) && (j+k)>i)?2:4 ) ) ) ) )) );
}





void foo2 (int i, int j, int k) {
    int trityp;
    if (i == 0 || j == 0 || k == 0) {
        trityp = 1;
    }
    else
    {
        if (i==2 && j==1 && k==1)
        {
            trityp = 4;
        }
        else
        {
            trityp = 0;
        }
        if (trityp == 0)
        {
            trityp = 1;
        }
        else
        {
            // error in the condition : trityp > 3 instead of trityp >= 3
            if (trityp >= 3)
            {
                trityp = 0;
            }
            else
            {
                trityp = 1;
            }
        }
    }
    assert(trityp==1);
}
