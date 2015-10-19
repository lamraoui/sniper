/**
 * SymbolicPath.h
 *
 * 
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/12/07
 * @copyright : NII 2013
 */


#ifndef _SYMBOLICPATH_H
#define _SYMBOLICPATH_H

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include "ExecutionEngine/Symbol.h"
#include "ExecutionEngine/SymbolMap.h"
#include "Logic/Expression.h"

using namespace llvm;


typedef struct State {
    bool branch;
    bool done;
} State_t;

//============================================================================
class SymbolicPath {
    
private:
    std::vector<ExprCellPtr> path;
    std::vector<ExprCellPtr> asserts;
    std::vector<ExprCellPtr> assumes; 
    std::vector<State_t>     stack;
    unsigned nbBranch;

public:
    SymbolicPath();
    SymbolicPath(SymbolicPath &obj);
    ~SymbolicPath();
    SymbolicPath* clone();
    
    std::vector<ExprCellPtr> getPath();
    std::vector<ExprCellPtr> getAsserts();
    std::vector<State_t>   getStack();
    void setStack(std::vector<State_t> s);

    void addBranchCell(SymbolPtr svcond, bool branchTaken, Instruction *i);
    void addCallCell(Value *vret, SymbolPtr sarg1, Instruction *i);
    void addAssertCell(SymbolPtr s, bool assertResult, Instruction *i);
    void addAssumeCell(SymbolPtr s, bool assumeResult, Instruction *i);
    unsigned getNbBranch();
    void clear();
    void clearStack() { stack.clear(); }
    void updateStack(bool branch);
    void dump();
    
private:
    void addCell(ExprCellPtr n);
    
};
//============================================================================

#endif
