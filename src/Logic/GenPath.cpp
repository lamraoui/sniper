/**
 * GenPath.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/03/29
 * @copyright : NII 2013
 */

#include "GenPath.h"


GenPath::GenPath() {
}


void GenPath::run(Function *main, Formula *formula) {
    
    // Compute the Adjacent matrix of the CFG
    BasicBlock *entryBB = &main->getEntryBlock();
    BasicBlock *returnBB = &main->back();
    nbBB = main->size();
    
    adjMatrix = (bool **)malloc(nbBB*sizeof(bool*));
    for(int x = 0; x < nbBB; x++) 
        adjMatrix[x] = (bool *)malloc(nbBB*sizeof(bool));
    
    // Compute a map: <BB to ID> and <ID to BB>
    unsigned id = 0; 
    std::map<BasicBlock*,unsigned> bb2id;
    std::map<unsigned,BasicBlock*> id2bb;
    for(Function::iterator it = main->begin(), e = main->end(); it != e; ++it) {
        BasicBlock *bb = it;
        bb2id[bb] = id;
        id2bb[id] = bb;
        id++;
    }
    
    int i = 0; 
    for(Function::iterator it = main->begin(), e = main->end(); it != e; ++it) {
        // Set the whole line to false
        for (int jj=0; jj<nbBB; jj++) {
            adjMatrix[i][jj] = false;
        }
        for (succ_iterator SI=succ_begin(it), E=succ_end(it); SI != E; ++SI) {
            BasicBlock *succBB = *SI;
            unsigned succID = bb2id[succBB];
            adjMatrix[i][succID] = true;
        }
        i++;
    }
    
    // Save the path during the recursive exploration
    path = (int*) malloc(nbBB*sizeof(int));
    // Lock for the taboo search (all init at false by default)
    taboo = (bool*) malloc(nbBB*sizeof(bool));
    for (int i=0; i<nbBB; i++) {
        path[i] = 0;
        taboo[i] = false;
    }
    // Choose the departure point and the target point
    source = bb2id[entryBB]; // entry block
    target = bb2id[returnBB]; // return block
    n = 0;
    
    // Enumerate all the paths 
    // (from the entry basicblock to the return basicblock)
    explore(source, 0);
    
    // --- Build all the formula ---------------------------
    std::vector<std::vector<int> >::iterator it1;
    for(it1=paths.begin(); it1!=paths.end(); ++it1) {
        std::vector<int> p = *it1;
        // Method2 : with negation of not executed BBs
        std::vector<ExprPtr> args_and;
        for (int id=0; id<nbBB; id++) {
            std::string bb;
            bb = id2bb[id]->getName();
            ExprPtr bb_expr = Expression::mkBoolVar(bb);
            // In the path?
            bool isInPath = (std::find(p.begin(),p.end(), id)!=p.end());
            if (isInPath) {
                args_and.push_back(bb_expr);
            } else {
                ExprPtr not_bb_expr = Expression::mkNot(bb_expr);
                args_and.push_back(not_bb_expr);
            }
        }
        if(args_and.size()>0) {
            ExprPtr and_expr = Expression::mkAnd(args_and);
            formulas.push_back(and_expr);
        }
    }
    
}


// ===========================================================================
// Depth First Search (DFS) with backtracking
//
// http://www.developpez.net/forums/d647090/autres-langages/algorithmes/mathematiques/parcours-chemins-graphe/
// ===========================================================================
void GenPath::explore(int position, int depth) {
    
	path[depth] = position;
    // We reach the target, we are done
	if (position==target) {
		// Print the solution
		//for(int i=0;i<=depth;i++) 
        //    std::cout << path[i] << " ";
        //std::cout << std::endl;
        // Save the path
        std::vector<int> p;
        for(int i=0;i<=depth;i++) 
            p.push_back(path[i]);
        paths.push_back(p);
		return;
	}
	// Else...
	taboo[position] = true; // on pose un caillou
	// on explore les chemins restants
	for(int i=0;i<nbBB;i++) {
		if (!adjMatrix[position][i] || taboo[i]) 
            continue;
        //std::cout << "d: " << i << std::endl;
		explore(i, depth+1);
	}
	taboo[position] = false; // on retire le caillou
}


std::vector<ExprPtr> GenPath::getFormulas() {
    return formulas;
}


GenPath::~GenPath() {

    free(path);
    free(taboo);
    for (int i=0; i<nbBB; i++) {
        free(adjMatrix[i]);
    }
}