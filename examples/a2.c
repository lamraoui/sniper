
int main(int arg) {
    
    int a[4];
    
    if(arg==1)
        a[0] = 100;
    else if(arg==2) {
        a[0] = 200;
    } else {
        a[0] = 300;
    }
    a[0] = 400;
    
    return a[0];
}

/*
 ----------------------------------------------------------------------
 [H]  (= bb_bb4 true)
 [H]  (= bb2_bb4 true)
 [H]  (= bb3_bb4 true)
 [H]  (= bb4_return true)
 [59]          (= tmp (= arg 1))
 [branch][65]  (and (= entry_bb tmp) (= entry_bb1 (not tmp)))
 [67]          (and (< 0 4) (= tmp1 (update a (0) 100)))
 [75]          (= tmp2 (= arg 2))
 [branch][81]  (and (= bb1_bb2 tmp2) (= bb1_bb3 (not tmp2)))
 [83]          (and (< 0 4) (= tmp3 (update a (0) 200)))
 [89]          (and (< 0 4) (= tmp4 (update a (0) 300)))
 [s_phi][95]   (= sphi12tmp4tmp3tmp1
 (ite (and bb bb_bb4) tmp1 (ite (and bb2 bb2_bb4) tmp3 tmp4))
 )
 [107]  (and (< 0 4) (= tmp5 (update sphi12tmp4tmp3tmp1 (0) 400)))
 [113]  (and (< 0 4) (= tmp7 (tmp5 0)))
 [117]  (= DUMMY_ tmp7)
 [121]  (= DUMMY_retval DUMMY_)
 ----------------------------------------------------------------------
 */ 



























/*
 ----------------------------------------------------------------------
 (= entry true)
 (= bb false)
 (= entry_bb false)
 (= bb1 true)
 (= entry_bb1 true)
 (= bb2 false)
 (= bb1_bb2 false)
 (= bb3 true)
 (= bb1_bb3 true)
 (= bb4 true)
 (= bb3_bb4 true)
 (= bb2_bb4 true)
 (= return true)
 (= bb4_return true)
 (= tmp false)
 (= bb_bb4 true)
 (= tmp2 false)
 (= arg 401)
 (= sphi12tmp4tmp3tmp1 tmp4)
 (= tmp7 400)
 (= DUMMY_ 400)
 (= DUMMY_retval 400)
 (= (tmp1 0) 100)
 (= (tmp3 0) 200)
 (= (tmp4 0) 300)
 (= (tmp5 0) 400)
 (= (a 0) 402)
 ----------------------------------------------------------------------
 */