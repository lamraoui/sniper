/**
 * \file GlobalVariables.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   29 January 2015
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

#include "Utils/Utils.h"

using namespace llvm;

/**
 * \class GlobalVariables
 *
 * This class provides a method to move global variable
 * declarations to the target function.
 */
class GlobalVariables {
    
private:
    /**
     * Map a LLVM global variable to its 
     * associated load/store instruction.
     */
    std::map<GlobalVariable*, Value*> gv2val;
    /**
     * Map a LLVM global variable to its 
     * associated alloca instruction.
     */
    std::map<Value*, AllocaInst*> gv2a;
    
public:
    GlobalVariables() { }
    /**
     * Destructor.
     */
    ~GlobalVariables() { }
    
    /**
     * Move global variable declarations of the target LLVM module 
     * to the entry basic block of the LLVM function \p targetFun.
     *
     * \param targetFun An LLVM function.
     */
    void process(Function *targetFun);
    
private:
    /**
     * Replace variables in load or store instructions that 
     * points to a global variables so that it points to 
     * the local declaration of the global variable.
     *
     * \param inst An LLVM load/store instruction.
     */
    void replacePointer(Instruction *inst);
    
};

#endif // _GLOBALVARIABLES_H
