/**
 * Combine.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/10/19
 * @copyright : NII 2015
 */

#ifndef _COMBINE_H
#define _COMBINE_H

#include <vector>

#include "Utils/HittingSet.h"
#include "Logic/Formula.h"
#include "Options.h"

//============================================================================
class Combine {
    
public:
    enum Method {
        MHS,
        PWU,
        FLA,
        NONE
    };
    
public:
    static SetOfFormulasPtr combineByMHS(std::vector<SetOfFormulasPtr> M);
    static SetOfFormulasPtr combineByPWU(std::vector<SetOfFormulasPtr> M);
    static SetOfFormulasPtr combineByFlatten(std::vector<SetOfFormulasPtr> M);
    
private:
    static void pairwiseUnion(std::vector<SetOfFormulasPtr> M,
                            SetOfFormulasPtr Diag);

};
//============================================================================

#endif
