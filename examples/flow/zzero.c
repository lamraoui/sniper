
void zZero (int input) {
    
    int x = 1;
    int z = 0;
    int y = input - 42;
    if (y < 0) {
        x = 0;
    } else if(y==0) {
        z = 1;
    }
    if (z==1) {
        x = 0;
    }
    
    assert(x!=0);
}




/*
 < 1 > Potential BUG @ :./flowse/zzero2.c:13
 x = 0;
 
 < 2 > Potential BUG @ :./flowse/zzero2.c:6
 int y = input - 42;
 
 < 3 > Potential BUG @ :./flowse/zzero2.c:7
 if (y < 0) {
 
*/ 

/*
==============================================
-- counterexample --
(= input 42)

==============================================

 cost: 1
 potential error locations (round 1) (cost 1):
 [131] 13 :         x = 0;

 potential error locations (round 2) (cost 2):
 [99] 9 :     } else if(y==0) {

 potential error locations (round 3) (cost 3):
 [105] 10 :         z = 1;

 potential error locations (round 4) (cost 4):
 [125] 12 :     if (z==1) {

 potential error locations (round 5) (cost 5):
 [85] 6 :     int y = input - 42;
 
 potential error locations (round 6) (cost 8):
 [89] 7 :     if (y < 0) {
 [95] 8 :         x = 0;

 ==============================================

 
 
 
 





/*======== V1 =============
1
2   void zZero (int input) {
3        
4    int x = 1;
5    int z = 0;
6    int y = input - 42;
7    if (y < 0) {
8        x = 0;
9    } else {
10        z = 1;
11    }
12    if (z==1) {
13        x = 0;
14    }
15    
16    assert(x!=0);
17  }
*/


/*
 
 < 1 > Potential BUG @ :./flowse/zzero.c:13
  x = 0;
 
 < 2 > Potential BUG @ :./flowse/zzero.c:12
  if (z==1) {
 
 < 3 > Potential BUG @ :./flowse/zzero.c:6
 int y = input - 42;
 
*/

/*
 
 ==============================================
 -- counterexample --
 (= input 42)
 
 ==============================================
 
 potential error locations (round 1) (cost 1):
 [109] 13 :         x = 0;
 
 potential error locations (round 2) (cost 2):
 [103] 12 :     if (z==1) {
 
 potential error locations (round 3) (cost 3):
 [87] 10 :         z = 1;
 
 potential error locations (round 4) (cost 5):
 [77] 7 :     if (y < 0) {
 [83] 8 :         x = 0;
 ==============================================
*/

/*
 (assert (= entry true))
 (assert (= bb (and entry entry_bb)))
 (assert (= bb1 (and entry entry_bb1)))
 (assert (= bb2
 (or (and (and bb1 bb1_bb2) (not (and bb bb_bb2)))
 (and (not (and bb1 bb1_bb2)) (and bb bb_bb2)))))
 (assert (= bb3 (and bb2 bb2_bb3)))
 (assert (= bb4
 (or (and (and bb3 bb3_bb4) (not (and bb2 bb2_bb4)))
 (and (not (and bb3 bb3_bb4)) (and bb2 bb2_bb4)))))
 (assert (= return (and bb4 bb4_return)))
 (assert (and (= entry_bb tmp4) (= entry_bb1 (not tmp4))))
 (assert (= bb bb_bb2))
 (assert (= bb1 bb1_bb2))
 (assert (= x.0 (ite (and bb bb_bb2) _x1 _x)))
 (assert (= z.0 (ite (and bb bb_bb2) _z _z2)))
 (assert (and (= bb2_bb3 tmp5) (= bb2_bb4 (not tmp5))))
 (assert (= bb3 bb3_bb4))
 (assert (= x.1 (ite (and bb3 bb3_bb4) _x3 x.0)))
 (assert (= tmp6 (/= x.1 0)))
 (assert (= tmp7 (ite tmp6 1 0)))
 (assert (= bb4 bb4_return))
 (assert (= input 42))
 
 (assert+ (= _x 1) 1)
 (assert+ (= _z 0) 1)
 (assert+ (= tmp (- input 42)) 1)
 (assert+ (= _y tmp) 1)
 (assert+ (= tmp4 (< _y 0)) 1)
 (assert+ (= _x1 0) 1)
 (assert+ (= _z2 1) 1)
 (assert+ (= tmp5 (= z.0 1)) 1)
 (assert+ (= _x3 0) 1)
 

*/