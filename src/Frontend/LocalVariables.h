/**
 * LocalVariables.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2012/03/11
 * @copyright : NII 2012
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


typedef struct varArg {
    Instruction *inst;
    std::string name;
    //Type *type;
    int pos;
} varArg_t;

typedef struct var {
    std::string name;
    int type;
} var_t;


//============================================================================
class LocalVariables {
    
public:
    std::vector<var_t> localVariables;
    
private:
    Function *mainFun;
    std::vector<varArg_t> vars; 
    std::map<std::string, unsigned> ptr2line;
    
public:
    LocalVariables(Function *_mainFun);
    void processLoadStore();
    void processPhi();
    std::string getPtr(Instruction *i, int pos);
    unsigned getLine(std::string name);
    ~LocalVariables();
    
private:
    void set(Instruction *i, LoadInst *load, int pos);
    bool hasEnding(std::string const &fullString, std::string const &ending);
    
};
//============================================================================

#endif
