/*
 Le programme BSearch prend en entrée un tableau des 
 entiers et un entier x. Par un processus dichotomique, 
 il permet de renvoyer l'indice de la case dans le tableau 
 dont la valeur est x. Dans le cas où x ne figure pas sur 
 le tableau, le programme retourne la valeur -1. 
 
 L'erreur dans ce programme est sur l'instruction d'affectation 
 "droite = milieu - 1" (ligne 20), l'instruction correcte devra 
 être "gauche = milieu + 1". En prenant l'entrée {tab[0]=-2147477728, 
 tab[1]=-2147470009, tab[2]=-2147468773, tab[3]=-2147466336, 
 tab[4]=-2147457481, tab[5]=-2147457481, tab[6]=-2147457481, 
 tab[7]=-2147457481, tab[8]=2147457481, tab[9]=-2147457480, 
 x=-2147457480}, le programme renvoie -1 indiquant que x=-2147457480 
 ne figure pas sur le tableau en entrée tab, alors qu'il devait 
 renvoyer l'indice 9 car la valeur de la neuvième case du tab 
 est égale à x.
 http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres
 */

int BSearchKO(int x) {
	int result = -1;int tab[10];int res; //__CPROVER_assume((x == -2147457480) && (tab[0]==-2147477728) && (tab[1]==-2147470009) && (tab[2]==-2147468773) && (tab[3]==-2147466336) && (tab[4]==-2147457481) && (tab[5]==-2147457481) && (tab[6]==-2147457481) && (tab[7]==-2147457481) && (tab[8]==2147457481) && (tab[9]==-2147457480));
	int milieu = 0;int length = 10;
	int gauche = 0;
	int droite = length - 1;
	while ((result == -1) && (gauche <= droite)) {
	    milieu = (gauche + droite) / 2;
	    if (tab[milieu] == x) {
            result = milieu;
	    }
	    else if (tab[milieu] <= x) {
            droite = milieu - 1; // wrong cute and paste : error in the assignment : should be "gauche = milieu + 1"
	    }
	    else {
            droite = milieu - 1; 
	    }
	}
	res=result; 
    assert(res == 9);
}
