
int absValue(int input) {
    
    //__CPROVER_assume(input==-2145385855);
    
    int sign, abs;
    abs = input;
    
	int ret;
    if (input==0) {
        ret = 0;
    } else {
        if (input<0) {
            sign = -1;
            printf("negative");
        } else {
            sign = 1;
            printf("positive");
        }
        if (sign==-1) {
            sign = input * -1;
        } else {
            abs = input;
        }
        ret = abs;
	}
    //assert(abs>=0);
    // (>= abs 0) and ((< index 0) <=> (= sign -1))
    // Logical equivalence:
    //(P ∧ Q) ∨ (¬Q ∧ ¬P)     ((P et Q) ou (non P et non Q)) 
    assert( ret==0 || (ret>0 && ((input<0 && sign==-1) || (input>=0 && sign==1))) );
    return ret;
}
