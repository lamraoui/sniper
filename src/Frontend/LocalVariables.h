/**
 * \file LocalVariables.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   29 January 2016
 */

#ifndef _LOCALVARIABLES_H
#define _LOCALVARIABLES_H

#include <string>
#include <iostream>

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Analysis/Verifier.h"

using namespace llvm;

/**
 * A variable of an LLVM instruction.
 */
typedef struct varArg {
    Instruction *inst; /*< Instruction that uses the variable */
    std::string name;  /*< Original variable name */
    int pos;           /*< Argument position of the variable in \a inst */
} varArg_t;


/**
 * \class LocalVariables
 *
 * While putting the IR into SSA form, LLVM automatically propagates 
 * constant values. This is something we want to avoid because we 
 * loose information and may encounter diffculties when mapping back
 * the IR’s instructions to the original source code.
 * To avoid this we pre-proccess the IR before putting it in SSA form. 
 * For each instruction we save the name of the used variables. 
 * Then, in the IR in SSA form, for each instruction containing a 
 * constant value, we check if the constant value refers to a variable. 
 * If this is the case, we replace the constant value by its variable 
 * name. We also assign these variables with the appropriate values.
 */
class LocalVariables {

private:
    /**
     * Vector of local variables information.
     */
    std::vector<varArg_t> vars;
    
public:
    LocalVariables() { }
    ~LocalVariables() { }
   
    /**
     * Process load and store instructions in \p mainFun so
     * that variables used in these instructions are not
     * propagated in other instructions during the SSA 
     * transformation pass (PromoteMemoryToRegisterPass).
     *
     * \param mainFun An function to be process.
     */
    void processLoadStore(Function *mainFun);
    /**
     * Get the name of a variable used in \p i at position \p pos.
     *
     * \param i An LLVM instruction.
     * \param pos Argument position [0:number of args in \p i[.
     */
    std::string getPtr(Instruction *i, int pos);
    
private:
    /**
     * Set the name of a variable used in \p i and \p load.
     * The variable is at position \ pos in \p i.
     *
     * \param i An LLVM instruction.
     * \param load A LLVM load instruction.
     * \param pos Argument position in \p i [0:number of args in \p i[.
     */
    void set(Instruction *i, LoadInst *load, int pos);
    
};

#endif // _LOCALVARIABLES_H
