/**
 * Combine.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/10/19
 * @copyright : NII 2015
 */

#include "Combine.h"


// Pair-wise union based combination
SetOfFormulasPtr Combine::combineByPWU(std::vector<SetOfFormulasPtr> D) {
    // Compute the MCSes
    std::vector<SetOfFormulasPtr> MCSesNoDoublons;
    for (SetOfFormulasPtr M : D) {
        M->removeDoublons();
        MCSesNoDoublons.push_back(M);
    }
    SetOfFormulasPtr combMCSes = SetOfFormulas::make();
    if (!MCSesNoDoublons.empty()) {
        // Pair-wise union of MCSes to obtain the complete diagnosis
        pairwiseUnion(MCSesNoDoublons, combMCSes);
        combMCSes->removeDoublons();
        combMCSes->removeSubsets();
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
SetOfFormulasPtr Combine::combineByMHS(std::vector<SetOfFormulasPtr> D) {
    // Compute the MUSes
    SetOfFormulasPtr MUSes = SetOfFormulas::make();
    for (SetOfFormulasPtr M : D) {
        M->removeDoublons();
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
        MUSes->add(MUS);
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
        allElts->add(M);
    }
    return allElts;
}
