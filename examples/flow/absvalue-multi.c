
int absValue(int input) {
    int sign, abs;
    abs = input;
    if (input==0) {
        return 0;
    }
    if (input<0) {
        sign = -1;
        printf("negative");
    } else {
        sign = -1; // BUG1
        printf("positive");
    }
    if (sign==-1) {
        sign = input * -1; //BUG2
    } else {
        abs = input;
    }
    
    // (>= abs 0) and ((< index 0) <=> (= sign -1))
    // Logical equivalence:
    //(P ∧ Q) ∨ (¬Q ∧ ¬P)     ((P et Q) ou (non P et non Q)) 
    assert( abs>=0 && ((input<0 && sign==-1) || (input>=0 && sign!=-1)) );
    
    return abs;
}
