/*
 Ce programme prend en entrée six variables à 
 valeurs entières et permet d'affecter:
 la plus grande valeur à la variable max ;
 et la plus petite valeur à la variable min.
 
 Ce programme contient 3 erreurs conditionnelles: lignes 12, 
 15 et 20. En prenant l'entrée suivante {a=1, b=-3, c=-4, 
 d=-2, e=-1, f=-2}, le programme entraîne la sortie suivante 
 {max=-1,min=-4} au lieu de {max=1,min=-4}.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

/*
 * Find the maximum and minimum of six values.
 */
void Maxmin6varKO4 (int a, int b, int c, int d, int e, int f) {//__CPROVER_assume((a==1) && (b==-3) && (c==-4) && (d==-2) && (e==-1) && (f==-2));
	int max;
    int min;
	if ((a>b) && (a>c) && (b>d) && (a>e) && (a>f)){ // error, the instruction should be ((a>b) && (a>c) && "(a>d)" && (a>e) && (a>f))
        max=a;
        /*Calculer le minimum*/
        if ((b>c) && (b<d) && (b<e) && (b<f)){ // error, the instruction should be ("(b<c)" && (b<d) && (b<e) && (b<f))
            min=b;
        }
        else{
            if ((c<d) && (c>e) && (c<f)){ // error, the instruction should be ((c<d) && "(c<e)" && (c<f))
                min=c;
            }
            else{
                if ((d<e) && (d<f)){
                    min=d;
                }
                else{
                    if (e<f){
                        min=e;
                    }
                    else{
                        min=f;
                    }
                }
            }  
        }
        
	}else{ 
	    if ((b>c) && (b>d) && (b>e) && (b>f)){
            max=b;
            
            if ((a<c) && (a<d) && (a<e) && (a<f)){
                min=a;
            }else{
                if ((c<d) && (c<e) && (c<f)){
                    min=c;
                }
                else{
                    if ((d<e) && (d<f)){
                        min=d; 
                    }
                    else{
                        if (e<f){
                            min=e;
                        }
                        else{
                            min=f;
                        }
                    }
                }  
            }
            
        }    
	    else{
            if ((c>d) && (c>e) && (c>f)){ 
                max=c;
                
                if ((a<b) && (a<d) && (a<e) && (a<f)){
                    min=a;
                }else{
                    if ((b<d) && (b<e) && (b<f)){
                        min=b;
                    }
                    else{
                        if ((d<e) && (d<f)){
                            min=d; 
                        }
                        else{
                            if (e<f){
                                min=e;
                            }
                            else{
                                min=f;
                            }
                        }
                    }  
                }
                
            }
            else{
                if ((d>e) && (d>f)){
                    max=d;
                    
                    if ((a<b) && (a<c) && (a<e) && (a<f)){
                        min=a;
                    }else{
                        if ((b<c) && (b<e) && (b<f)){
                            min=b;
                        }
                        else{
                            if ((c<e) && (c<f)){
                                min=c; 
                            }
                            else{
                                if (e<f){
                                    min=e;
                                }
                                else{
                                    min=f;
                                }
                            }
                        }  
                    }
                }
                else{
                    if (e>f){
                        max=e;
                        
                        if ((a<b) && (a<c) && (a<d) && (a<f)){
                            min=a;
                        }else{
                            if ((b<c) && (b<d) && (b<f)){
                                min=b;
                            }
                            else{
                                if ((c<d) && (c<f)){
                                    min=c; 
                                }
                                else{
                                    if (d<f){
                                        min=d;
                                    }
                                    else{
                                        min=f;
                                    }
                                }
                            }  
                        }       
                    }
                    else{
                        max=f;
                        
                        if ((a<b) && (a<c) && (a<d) && (a<e)){
                            min=a;
                        }else{
                            if ((b<c) && (b<d) && (b<e)){
                                min=b;
                            }
                            else{
                                if ((c<d) && (c<e)){
                                    min=c; 
                                }
                                else{
                                    if (d<e){
                                        min=d;
                                    }else{
                                        min=e;
                                    }
                                }
                            }  
                        } 
                    }
                }
            }
	    }
    }
    assert( (max >= a) && (max >= b) && (max >= c) && (max >= d) && (max >= e) && (max >= f) && (min <= a) && (min <= b) && (min <= c) && (min <= d) && (min <= e) && (min <= f));                  
}