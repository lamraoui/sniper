/*
 Ce programme a été utilisé dans le papier "Visualization of Test 
 Information to Assist Fault Localization" pour introduire et 
 expliquer l'approche Tarantula. Nous avons utilisé la version 
 erronée utilisée dans le papier mentionné.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres

 find the median of three variables.
 */

void MidKO (int a, int b, int c) {//__CPROVER_assume((a==2) && (b==1) && (c==3));
	int m;
	m=c;
	if (b<c){
	    if (a<b){
            m=b;
	    }
	    else if (a<c){
            m=b; // error, the instruction should be m=a;
	    }          
	}
	else{
	    if (a>b){
            m=b;
	    }
	    else if (a>c){
            m=a;
	    }
	}	
    assert(m==2);
}

      
       
     

      
   