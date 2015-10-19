/*
 Le programme Minmax prend en entrée trois entiers et 
 permet de d'affecter la plus petite valeur à la variable 
 least et la plus grande valeur à la variable most. 
 
 L'erreur dans ce programme est sur l'instruction 
 d'affectation "most = in2" (ligne 19), l'instruction 
 devra être "least = in2". Pour une entrée égale à 
 {in1=2, in2=1, in3=3}, on devra avoir comme sortie 
 least=1 et most=3. Avec cette entrée le programme 
 sort least=2 et most=1, ce qui est non-conforme avec 
 la postcondition posée "(least <= most)".
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

extern void assert(int x);
    
void foo(int in1,int in2,int in3){
	int least;//__CPROVER_assume((in1==2) && (in2==1) && (in3==3));
	int most;
    least = in1;
	most = in1;
	if (most < in2){
	    most = in2;
	}
	if (most < in3){
	    most = in3;
	}
	if (least > in2){ 
	    most = in2; // error in the assignment : most = in2 instead of least = in2
	}
	if (least > in3){ 
	    least = in3; 
	}
    assert(least <= most);
}

      
      

