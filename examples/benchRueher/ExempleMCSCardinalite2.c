/*
 Ce programme prend en entrée une variable i, et il permet de calculer 
 l'affectation suivante :"z=i*(|v|+|w|)". En sortie il faudrait 
 respecter la postcondition suivante: "((z==0||i!=0)&&(v>=0)&&(w>=0))".
 
 L'erreur dans ce programme porte sur l'instruction "z=i+(|v|+|w|)", 
 l'instruction correcte devra être "z=i*(|v|+|w|)". En prenant comme 
 entrée {i=0} le programme échoue et viole sa postcondition.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

void ExempleMCSCardinalite2(int i) {
    int v,w,z;i=0;
    v=5;
    w=6;
    z=i+(v+w); // error in the assignment : z=i+(v+w); instead of z=i*(v+w);
    assert((z==0 || i!=0) && (v>=0) && (w>=0));
}
