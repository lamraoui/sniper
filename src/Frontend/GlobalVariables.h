/**
 * GlobalVariables.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/02/13
 * @copyright : NII 2013
 */

#ifndef _GLOBALVARIABLES_H
#define _GLOBALVARIABLES_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Operator.h"
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Support/InstIterator.h>

#include "Utils/utils.h"

using namespace llvm;


//============================================================================
class GlobalVariables {
    
private:
    bool dbgMsg;
    Function *mainFun;
    std::map<GlobalVariable*, Value*> gv2val;
    std::map<Value*, AllocaInst*> gv2a;
    
public:
    GlobalVariables(Function *_mainFun, bool _dbgMsg);
    ~GlobalVariables() { }
    void process();
    
private:
    void replacePointer(Instruction *inst);
    
};
//============================================================================

#endif
