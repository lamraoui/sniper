Bf[5-20], Bf[5-20]V2
Les programmes Bf[5-20]V2 sont inspirés des programmes qu'on a trouvés dans la base de benchs de LLBMC (Bf[5-20]). Les programmes Bf[5-20] représentent une implémentation de l'algorithme de Dijkstra en fixant le nombre de nœuds dans le graphe et en faisant varier le nombre d'arcs, leurs poids et leurs sommets de départ et de destinations. Ils permettent de calculer le plus court chemin entre le sommet source (noeud 0) et n'importe quel noeud dans le graphe pris en entrée. 

L'erreur dans ces programmes est sur l'instruction d'affectation "distance[i] = -1" (ligne 24), l'instruction correcte devra être "distance[i] = 0": l'erreur provoquée indique que la distance entre le sommet source et lui-même est -1, or elle devra être égale à 0.

http://www.i3s.unice.fr/~bekkouch/Bench_Mohammed.html#benres