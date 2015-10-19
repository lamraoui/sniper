/**
 * HittingSet.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/03/01
 * @copyright : NII 2013
 */


// http://stackoverflow.com/questions/7936037/any-good-implementation-of-greedy-set-cover-for-large-datasets


#include "HittingSet.h"





//
// xi is equal to 1 iff Si is selected
//
// 
void HittingSet::getMinimalHittingSets_LP(std::vector<std::set<unsigned> > &S,
                                          std::vector<std::set<unsigned> > &MHS) {
        
    // Create the universe U
    std::set<unsigned> U;
    for (unsigned i=0; i<S.size(); i++) {
        U.insert(S[i].begin(), S[i].end());
    }
    // Create a boolean variables for each elements in U
    std::map<unsigned, unsigned> elt2id;
    std::vector<yices_expr> X(U.size());
    std::vector<yices_var_decl> Xdecl(U.size());
    yices_context ctx = yices_mk_context();
    unsigned i = 0;
    std::set<unsigned>::const_iterator itu;
    for (itu=U.begin(); itu!=U.end(); ++itu) {
        unsigned elt = *itu;
        elt2id[elt] = i;
        std::stringstream ss;
        ss << "X" << i;
        Xdecl[i] = yices_mk_bool_var_decl(ctx, ss.str().c_str());
        X[i]     = yices_mk_bool_var_from_decl(ctx, Xdecl[i]);
        // Construct the weighted constraints ((not xi) 1)
        yices_expr eq_expr =  yices_mk_not(ctx, X[i]);
        yices_assert_weighted(ctx, eq_expr, 1);
        i++;
    }
    
    // For each set in S, 
    // create a hard constraint: (or x1 x2 ... xn) 
    // where xi is present in the constraint iff 
    // xi hits the current set
    unsigned j = 0;
    std::map<unsigned, std::set<yices_expr> > setid2Xs;
    std::vector<std::set<unsigned> >::const_iterator it1;
    for (it1=S.begin(); it1!=S.end(); ++it1) {
        std::set<unsigned>::const_iterator it2;
        for (it2=(*it1).begin(); it2!=(*it1).end(); ++it2) {
            // element : (xi ... xj)
            setid2Xs[j].insert(X[elt2id[*it2]]);
        }
        j++; // Next set
    }
    // Construct the constraints (or xi ... xj)
    std::map<unsigned, std::set<yices_expr> >::const_iterator it3;
    for (it3=setid2Xs.begin(); it3!=setid2Xs.end(); ++it3) {
        std::set<yices_expr> Xs = it3->second;
        yices_expr or_expr;
        if (Xs.size()==1) {
            or_expr = *Xs.begin();
        } else {
            yices_expr orTab[Xs.size()];
            unsigned j = 0;
            std::set<yices_expr>::const_iterator it4;
            for (it4=Xs.begin(); it4!=Xs.end(); ++it4) {
                yices_expr e = *it4;
                orTab[j] = e;
                j++;
            }
            or_expr = yices_mk_or(ctx, orTab, Xs.size());  
        }
        yices_assert(ctx, or_expr);
    }
    
    bool done =false;
    while (!done) {
        // Solve the formula
        //std::cout << "max-sat\n";
        switch(yices_max_sat(ctx)) {
            case l_true: {
                // Display the model
                yices_model model = yices_get_model(ctx);
                //yices_display_model(model);
                // Get the value from the model for each Xi
                unsigned j = 0;
                yices_expr args[U.size()];
                std::set<unsigned> subset;
                for (unsigned i=0; i<U.size(); i++) {
                    lbool val = yices_get_value(model, Xdecl[i]);
                    // The set Si was selected
                    if (val==l_true) { 
                        // Save the element
                        std::set<unsigned>::iterator it = U.begin();
                        std::advance(it, i); // advanced by i
                        subset.insert(*it);
                        // To block the current solution
                        args[j++] = yices_mk_not(ctx, X[i]);
                    }
                }
                if (!subset.empty()) {
                    // Save the solution
                    MHS.push_back(subset);
                    // Block the current solution
                    yices_expr blockExpr = yices_mk_or(ctx, args, j);
                    yices_assert(ctx, blockExpr);
                } else {
                    done = true;
                }
            } break;
            case l_false: // unsatisfiable
                done = true;
                break;
            case l_undef: // unknown
                done = true;
                break;
        }    
        
    }
    yices_del_context(ctx);
}




// F : collection of set
void HittingSet::getMinimalHittingSets(std::vector<std::set<unsigned> > &F,
                                       std::vector<std::set<unsigned> > &MHS) {
    if (F.empty()) {
        return;
    }
    // Init the root node
    Node_t *n = new Node_t();
    n->set = F[0];
    n->edgeVal = 0;
    n->pred = NULL;
    Node_t *root = n;
    
    // Compute the Minimal Hitting Sets
    computeHittingSetDAG(F, root, root);
    
    // Get the sets from the tree
    std::vector<std::set<unsigned> > MHS_tmp;
    unsigned path[1000];
    saveTree(root, path, 0, MHS_tmp);
    
    std::vector<std::set<unsigned> >::iterator it;
    for(it=MHS_tmp.begin(); it!=MHS_tmp.end(); ++it) {
        std::set<unsigned> S1 = *it;
        bool ok = true;
        std::vector<std::set<unsigned> >::iterator it2;
        for(it2=MHS_tmp.begin(); it2!=MHS_tmp.end(); ++it2) {
            std::set<unsigned> S2 = *it2;
            // Remove the over-sets
            if (S1!=S2 && std::includes(S1.begin(), S1.end(), S2.begin(), S2.end()))
            {
                ok = false;
            }
        }
        if (ok) {
            // Remove the doublons
            bool is_in = false;
            std::vector<std::set<unsigned> >::iterator it2;
            for(it2=MHS.begin(); it2!=MHS.end(); ++it2) {
                if (*it2==S1) {
                    is_in = true;
                    break;
                }
            }
            if (!is_in) {
                MHS.push_back(S1);
                //printSet(S1);
            }
        }
    }
}

void HittingSet::computeHittingSetDAG(std::vector<std::set<unsigned> > &F,
                                      Node_t *root, Node_t *node) {
    
    std::set<unsigned>::iterator it2;
    for(it2=(node->set).begin(); it2!=(node->set).end(); ++it2) {
        unsigned v = *it2;
        
        // Compute H(n_succ)
        std::set<unsigned> H;
        if (v>0) {
            H.insert(v);
        }
        Node_t *current = node;
        while (current!=root) {
            if (current->edgeVal>0) {
                H.insert(current->edgeVal);
            }
            current = current->pred;
        }
        
        // Find K such that KnH(n_succ) = {}
        std::set<unsigned> K;
        std::vector<std::set<unsigned> >::iterator it4;
        for(it4=F.begin(); it4!=F.end(); ++it4) {
            std::set<unsigned> s = *it4;
            std::set<unsigned> intersect;
            set_intersection(s.begin(),s.end(),H.begin(),H.end(),
                             std::inserter(intersect,intersect.begin()));
            if (intersect.empty()) {
                K = s;
                break;
            }
        }
    
        Node_t *next = new Node_t();
        next->edgeVal = v;
        next->pred = node;
        (node->nexts).push_back(next);
        if (!K.empty()) {
            next->set = K;
            computeHittingSetDAG(F, root, next);
        }   
    }
}


void HittingSet::saveTree(Node_t *node, unsigned path[], unsigned len,
                          std::vector<std::set<unsigned> > &MHS_tmp) {
    
    path[len] = node->edgeVal;
    if ((node->nexts).size()==0) {
        std::set<unsigned> S;
        for (unsigned i=0; i<=len; i++) {
            if (path[i]>0) {
                S.insert(path[i]);
            }
        }
        MHS_tmp.push_back(S);
    } else {
        std::vector<Node_t*>::iterator it;
        for(it=(node->nexts).begin(); it!=(node->nexts).end(); ++it) {
            Node_t *n = *it;
            saveTree(n, path, len+1, MHS_tmp); 
        }
    }
}


void HittingSet::printSet(std::set<unsigned> S) {
    std::cout << "{";
    std::set<unsigned>::iterator it;
    for(it=S.begin(); it!=S.end(); ++it) {
        unsigned n = *it;
        std::cout << n << " ";
    } 
    std::cout << "}" << std::endl;
}