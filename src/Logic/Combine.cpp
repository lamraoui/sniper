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
        //if (options->printMUS()) {
        //    FormulaPtr MUS = Formula::make(); // MUS
        //    HittingSet::getMinimalHittingSets_LP(M, MUS);
        //    std::cout << "MUS: " << MUS << "\n\n";
        //}
    }
    SetOfFormulasPtr combMCSes = SetOfFormulas::make();
    if (!MCSesNoDoublons.empty()) {
        //removeDoublons(MCSesNoDoublons);
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
            S->add(B);
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
/*    // Compute the MUSes
    SetOfFormulasPtr MUSes;
    for (SetOfFormulasPtr M : D) {
        M->removeDoublons();
        // Compute a MUS with the MHS of a MCS
        SetOfFormulasPtr MUS;
        HittingSet::getMinimalHittingSets_LP(M, MUS);
        // Print MUS
        //if (options->printMUS()) {
        //    std::cout << "MUS: " << MUS << "\n\n";
        //}
        MUSes->add(MUS); //MUSes.insert(MUSes.end(), MUS.begin(), MUS.end());
    }
    
    // Remove doublons
    MUSes->removeDoublons();
    //std::sort(MUSes.begin(), MUSes.end());
    //MUSes.erase(std::unique(MUSes.begin(), MUSes.end()), MUSes.end());
    
    // ACSR
    //const unsigned loc = options->getNbLOC();
    //if (loc>0) {
    //    std::cout << "ACSR: " << getCodeSizeReduction(MUSes, loc) << "%\n";;
    //}
    // Minimal hitting set of the union of the MUSes
    SetOfFormulasPtr combMCSes = SetOfFormulas::make();
    HittingSet::getMinimalHittingSets_LP(MUSes, combMCSes);*/
    
    SetOfFormulasPtr combMCSes = SetOfFormulas::make();
    
    
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
