/**
 * \file HittingSet.h
 *
 * ----------------------------------------------------------------------
 *                SNIPER : Automatic Fault Localization 
 *
 * Copyright (C) 2016 Si-Mohamed LAMRAOUI
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program (see LICENSE.TXT).  
 * If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 * \author Si-Mohamed Lamraoui
 * \date   25 January 2015
 */

#ifndef _HITTINGSET_H
#define _HITTINGSET_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "yices_c.h"

/**
 * \class HittingSet
 *
 * \brief This class provides a method to compute minimal hitting-set.
 */
template <typename T>
class HittingSet {
    
public:
    /**
     * Compute the minimal hitting-set \p H of \p S.
     *
     * <b>Definition (Hitting Set)</b> - \f$H\f$ is a hitting
     * set of \f$S\f$ iff \f$H \subseteq D\f$ and
     * \f$\forall S \in \Omega : H \cap S \neq \emptyset\f$.
     *
     * Let \p S be a collection of sets from some finite
     * domain \f$D\f$, a hitting set of \p S is a set of elements
     * from \f$D\f$ that covers (hits) every set in \p S by having
     * at least one element in common with it. A minimal hitting 
     * set is a hitting set from which no element can be removed 
     * without losing the hitting set property. There exist many 
     * algorithms to compute the hitting set. We here use a 
     * technique based on the Maximum Satisfiability.
     *
     * \param S A vector of sets (MCSes) (input).
     * \param H A vector of sets (MCSes) (output).
     */
    static void getMinimalHittingSets_LP(std::vector<std::set<T> > &S,
                                  std::vector<std::set<T> > &H);

};

// xi is equal to 1 iff Si is selected
template<class T>
void HittingSet<T>::getMinimalHittingSets_LP(std::vector<std::set<T> > &S,
                                             std::vector<std::set<T> > &H) {
    // Create the universe U
    std::set<T> U;
    for (unsigned i=0; i<S.size(); i++) {
        U.insert(S[i].begin(), S[i].end());
    }
    // Create a boolean variables for each elements in U
    std::map<T, unsigned> elt2id;
    std::vector<yices_expr> X(U.size());
    std::vector<yices_var_decl> Xdecl(U.size());
    yices_context ctx = yices_mk_context();
    unsigned i = 0;
    typename std::set<T>::const_iterator itu;
    for (itu=U.begin(); itu!=U.end(); ++itu) {
        T elt = *itu;
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
    typename std::vector<std::set<T> >::const_iterator it1;
    for (it1=S.begin(); it1!=S.end(); ++it1) {
        typename std::set<T>::const_iterator it2;
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
                std::set<T> subset;
                for (unsigned i=0; i<U.size(); i++) {
                    lbool val = yices_get_value(model, Xdecl[i]);
                    // The set Si was selected
                    if (val==l_true) {
                        // Save the element
                        typename std::set<T>::iterator it = U.begin();
                        std::advance(it, i); // advanced by i
                        subset.insert(*it);
                        // To block the current solution
                        args[j++] = yices_mk_not(ctx, X[i]);
                    }
                }
                if (!subset.empty()) {
                    // Save the solution
                    H.push_back(subset);
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

#endif // _HITTINGSET_H
