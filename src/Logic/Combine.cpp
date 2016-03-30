/**
 * \file Combine.cpp
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
 * \date   15 January 2015
 */

#include "Combine.h"


// Pair-wise union based combination
SetOfFormulasPtr Combine::combineByPWU(std::vector<SetOfFormulasPtr> D) {
    // TODO: remove subsets in D
    /*std::vector<SetOfFormulasPtr> MCSesNoDoublons;
    for (SetOfFormulasPtr M : D) {
        M->removeDoublons();
        MCSesNoDoublons.push_back(M);
    }*/
    // Compute the MCSes
    SetOfFormulasPtr combMCSes = SetOfFormulas::make();
    if (!D.empty()) {
        // Pair-wise union of MCSes to obtain the complete diagnosis
        pairwiseUnion(D, combMCSes);
    }
    return combMCSes;
}


// [[[int]]] -> [[int]]
// [[mcs]]   -> [[mcs]]
void Combine::pairwiseUnion(std::vector<SetOfFormulasPtr> MCSes,
                            SetOfFormulasPtr Diag) {
    // Init
    int size = MCSes.size();
    unsigned a[size];
    for (int i=0; i<size; ++i) {
        a[i] = 0;
    }
    bool done = false;
    while (!done) {
        // Unions
        FormulaPtr S = Formula::make();
        for (int i=0; i<size; ++i) {
            SetOfFormulasPtr A = MCSes[i];
            int j = a[i];
            FormulaPtr B = A->getAt(j);
            S->add(B->getExprs());
        }
        Diag->add(S);
        // Update indexes
        a[0]++;
        for (int i=0; i<size-1; ++i) {
            SetOfFormulasPtr A = MCSes[i];
            if(a[i]>=A->size()) {
                a[i] = 0;
                a[i+1]++;
            }
        }
        // Finished?
        SetOfFormulasPtr A = MCSes[size-1];
        if (a[size-1]>=A->size()) {
            done = true;
            break;
        }
    }
}

// Minimal-hitting set based combination
SetOfFormulasPtr Combine::combineByMHS(std::vector<SetOfFormulasPtr> D, 
                                       unsigned totalNbLine) {
    // Compute the MUSes
    SetOfFormulasPtr MUSes = SetOfFormulas::make();
    for (SetOfFormulasPtr M : D) {
        // Compute a MUS with the MHS of a MCS
        std::vector<std::set<ExprPtr> > InMCS;
        std::vector<std::set<ExprPtr> > OutMUS;
        std::vector<FormulaPtr> F = M->getFormulas();
        for (FormulaPtr f : F) {
            std::vector<ExprPtr> E = f->getExprs();
            std::set<ExprPtr> Eset(E.begin(), E.end());
            InMCS.push_back(Eset);
        }
        HittingSet<ExprPtr>::getMinimalHittingSets_LP(InMCS, OutMUS);
        SetOfFormulasPtr MUS = SetOfFormulas::make();
        for (std::set<ExprPtr> s : OutMUS) {
            FormulaPtr f = Formula::make();
            std::vector<ExprPtr> Evec;
            std::copy(s.begin(), s.end(), std::back_inserter(Evec));
            f->add(Evec);
            MUS->add(f);
        }
        MUSes->add(MUS->getFormulas());
    }
    // Minimal hitting set of the union of the MUSes
    std::vector<std::set<ExprPtr> > InMUSes;
    std::vector<std::set<ExprPtr> > OutCombMCSes;
    std::vector<FormulaPtr> F2 = MUSes->getFormulas();
    for (FormulaPtr f : F2) {
        std::vector<ExprPtr> E = f->getExprs();
        std::set<ExprPtr> Eset(E.begin(), E.end());
        InMUSes.push_back(Eset);
    }
    // ACSR
    if (totalNbLine>0) {
        std::cout << "ACSR: " << getCodeSizeReduction(InMUSes, totalNbLine) << "%\n";;
    }
    HittingSet<ExprPtr>::getMinimalHittingSets_LP(InMUSes, OutCombMCSes);
    SetOfFormulasPtr combMCSes = SetOfFormulas::make();
    for (std::set<ExprPtr> s : OutCombMCSes) {
        FormulaPtr f = Formula::make();
        std::vector<ExprPtr> Evec;
        std::copy(s.begin(), s.end(), std::back_inserter(Evec));
        f->add(Evec);
        combMCSes->add(f);
    }
    return combMCSes;
}

// Put all elements in MCSes into a single set
// {{{x},{x,y}},{{z}}} -> {x,y,z}
SetOfFormulasPtr Combine::combineByFlatten(std::vector<SetOfFormulasPtr> D) {
    SetOfFormulasPtr allElts = SetOfFormulas::make();
    for (SetOfFormulasPtr M : D ) {
        allElts->add(M->getFormulas());
    }
    return allElts;
}

double Combine::getCodeSizeReduction(std::vector<std::set<ExprPtr> > MUSes, 
                                     unsigned totalNbLine) {

    std::vector<double> CSR(MUSes.size());
    unsigned i = 0;
    for (std::set<ExprPtr> mus : MUSes) {
        CSR[i] = ((100.0*(double)mus.size())/(double)totalNbLine);
        i++;
    }
    double sum = 0;
    std::vector<double>::const_iterator it2;
    for (it2=CSR.begin(); it2!=CSR.end(); ++it2) {
        double crs_i = *it2;
        sum = sum + crs_i;
    }
    return sum/(double)CSR.size();
}
