
int main() {
    
    int x;
    int a[4];
    
    a[0] = 100;
    x = a[0];
    a[0] = 200;
    x = a[0];
    a[0] = 300;
    x = a[0];
    
    return a[0];
}

/*
 ------------------------------------------------------------
 [15]  (and (< 0 4) (= tmp (update a (0) 100)))
 [21]  (and (< 0 4) (= tmp4 (tmp 0)))
 [25]  (= DUMMY_x tmp4)
 [29]  (and (< 0 4) (= tmp5 (update tmp (0) 200)))
 [35]  (and (< 0 4) (= tmp7 (tmp5 0)))
 [39]  (= DUMMY_x1 tmp7)
 [43]  (and (< 0 4) (= tmp8 (update tmp5 (0) 300)))
 [49]  (and (< 0 4) (= tmp10 (tmp8 0)))
 [53]  (= DUMMY_x2 tmp10)
 [57]  (and (< 0 4) (= tmp12 (tmp8 0)))
 [61]  (= DUMMY_ tmp12)
 [65]  (= DUMMY_retval DUMMY_)
 [H]   (= entry_return true)
 ------------------------------------------------------------
*/

/*
 ------------------------------------------------------------
 (= entry true)
 (= return true)
 (= entry_return true)
 (= tmp4 100)
 (= DUMMY_x 100)
 (= tmp7 200)
 (= DUMMY_x1 200)
 (= tmp10 300)
 (= DUMMY_x2 300)
 (= tmp12 300)
 (= DUMMY_ 300)
 (= DUMMY_retval 300)
 (= (tmp 0) 100)
 (= (tmp5 0) 200)
 (= (tmp8 0) 300)
 ------------------------------------------------------------
*/