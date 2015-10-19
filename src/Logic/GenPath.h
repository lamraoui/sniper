/**
 * GenPath.h
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/03/29
 * @copyright : NII 2013
 */


#ifndef _GENPATH_H
#define _GENPATH_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Support/InstIterator.h>

#include "Logic/Formula.h"
#include "Logic/Expression.h"

using namespace llvm;


//============================================================================
class GenPath {
    
    
private:
    int target;
    int source;
    int nbBB;
    int *path;
    bool *taboo;
    bool **adjMatrix;
    // Paths (after computation)
    int n;
    std::vector<std::vector<int> > paths;
    std::vector<ExprPtr> formulas;
    
public:
    GenPath();
    void run(Function *main, Formula *formula);
    std::vector<ExprPtr> getFormulas();
    ~GenPath();
    
private:
    void explore(int position, int depth);
    
};
//============================================================================

#endif
