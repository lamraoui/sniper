
int main(int arg) {
    
    int x;
    int a[4];
    
    if (arg==2) {
    	
        if(arg==1)
            a[0] = 100;
        else 
            a[0] = 200;
    
    } else {
        x = a[0];
    }

    return a[0];
}

/*
----------------------------------------------------------------------
 [H]  (= bb1_bb3 true)
 [H]  (= bb2_bb3 true)
 [H]  (= bb3_bb5 true)
 [H]  (= bb4_bb5 true)
 [H]  (= bb5_return true)
 [73]          (= tmp (= arg 2))
 [branch][79]  (and (= entry_bb tmp) (= entry_bb4 (not tmp)))
 [81]          (= tmp1 (= arg 1))
 [branch][87]  (and (= bb_bb1 tmp1) (= bb_bb2 (not tmp1)))
 [89]          (and (< 0 4) (= tmp2 (update a (0) 100)))
 [95]          (and (< 0 4) (= tmp3 (update a (0) 200)))
 [s_phi][101]  (= sphi11tmp3tmp2 (ite (and bb1 bb1_bb3) tmp2 tmp3))
 [109]         (and (< 0 4) (= tmp5 (a 0)))
 [113]         (= DUMMY_x tmp5)
 [117]         (and (< 0 4) (= tmp7 (sphi11tmp3tmp2 0)))
 [121]         (= DUMMY_ tmp7)
 [125]         (= DUMMY_retval DUMMY_)
 ----------------------------------------------------------------------
 */

























/*
 ----------------------------------------------------------------------
 (= entry true)
 (= bb false)
 (= entry_bb false)
 (= bb1 false)
 (= bb_bb1 false)
 (= bb2 false)
 (= bb_bb2 true)
 (= bb3 false)
 (= bb2_bb3 true)
 (= bb1_bb3 true)
 (= bb4 true)
 (= entry_bb4 true)
 (= bb5 true)
 (= bb4_bb5 true)
 (= bb3_bb5 true)
 (= return true)
 (= bb5_return true)
 (= tmp false)
 (= tmp1 false)
 (= arg 201)
 (= sphi11tmp3tmp2 tmp3)
 (= tmp5 202)
 (= DUMMY_x 202)
 (= tmp7 200)
 (= DUMMY_ 200)
 (= DUMMY_retval 200)
 (= (tmp2 0) 100)
 (= (tmp3 0) 200)
 (= (a 0) 202)
 ----------------------------------------------------------------------
 */
