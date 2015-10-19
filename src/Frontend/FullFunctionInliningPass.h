/**
 * FullFunctionInliningPass.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/03/26
 * @copyright : NII 2013
 */

#ifndef FullFunctionInliningPass_H
#define FullFunctionInliningPass_H

#include <iostream>
#include <string>

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/DebugInfo.h"
#include "llvm/PassManagers.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Analysis/Verifier.h"

#include "Options.h"
#include "Utils/utils.h"

using namespace llvm;


//============================================================================
class FullFunctionInliningPass : public FunctionPass {
    
    static char ID;
    
private:
    Options *options;
    
public:
    FullFunctionInliningPass(Options *_options) 
    : FunctionPass(ID), options(_options) { }
    ~FullFunctionInliningPass() { }
    Pass *createFullFunctionInliningPass(Options *o);
    virtual bool runOnFunction(Function &F);
    
};
//============================================================================


#endif