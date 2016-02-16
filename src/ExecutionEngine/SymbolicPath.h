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
#include "Logic/Expression.h"

using namespace llvm;

class ExprCell;
class BranchExprCell;
class CallExprCell;
class AssertExprCell;
class AssumeExprCell;

typedef std::shared_ptr<ExprCell> ExprCellPtr;
typedef std::shared_ptr<BranchExprCell> BranchExprCellPtr;
typedef std::shared_ptr<CallExprCell> CallExprCellPtr;
typedef std::shared_ptr<AssertExprCell> AssertExprCellPtr;
typedef std::shared_ptr<AssumeExprCell> AssumeExprCellPtr;

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

class ExprCell {
protected:
    ExprPtr expr;
    ExprPtr notExpr;
    Instruction *i;
public:
    ExprCell(Instruction *i) : i(i) { }
    virtual ~ExprCell() { }
    virtual bool isBranch() { return false; }
    virtual bool isFunCall() { return false; }  
    virtual bool isAssert() { return false; } 
    virtual bool isAssume() { return false; }
    virtual void dump() = 0;
    ExprPtr getExpr() {
        return expr;
    }
    ExprPtr getNotExpr() {
        return notExpr;
    }
    Instruction* getInstruction() { 
        return i; 
    }
};

class BranchExprCell : public ExprCell {
protected:
    bool branchTaken;
public:
    BranchExprCell(SymbolPtr svcond, bool branchTaken, Instruction *i) 
    : ExprCell(i), branchTaken(branchTaken) {
        this->expr    = svcond->convertToExpr();
        this->notExpr = Expression::mkNot(this->expr);
    }
    bool getBranchTaken() { return branchTaken; }
    void setBranchTaken(bool branch) { branchTaken = branch; }
    virtual bool isBranch() { return true; }
    virtual void dump() {
        std::cout << "(BN: "; 
        expr->dump();
        std::cout << " )";
        if (branchTaken) {
            std::cout << " [T] ";
        } else {
            std::cout << " [F] ";
        }
    }
};

class CallExprCell : public ExprCell {
protected:
    Value *vret;
    std::vector<SymbolPtr> sargs;
    std::vector<Value*> vargs;
public:
    CallExprCell(Value *vret, Value *varg1, Instruction *i) 
    : ExprCell(i), vret(vret) { 
        vargs.push_back(varg1);
    }
    CallExprCell(Value *vret, std::vector<Value*> vargs, Instruction *i) 
    : ExprCell(i), vret(vret), vargs(vargs) { 
    }
    CallExprCell(Value *vret, SymbolPtr sarg1, Instruction *i) 
    : ExprCell(i), vret(vret) {
        sargs.push_back(sarg1);
    }
    CallExprCell(Value *vret, std::vector<SymbolPtr> sargs, Instruction *i) 
    : ExprCell(i), vret(vret), sargs(sargs) { 
    }
    Value* getRetValue() { return vret; }
    std::vector<Value*> getArgsValues() { return vargs; }
    std::vector<SymbolPtr> getArgsSymbols() { return sargs; }
    virtual bool isFunCall() { return true; }
    ExprPtr getExpr() {
        std::cout << "error: no expression for function node!\n";
        exit(1);
    }
    ExprPtr getNotExpr() {
        std::cout << "error: no expression for function node!\n";
        exit(1);
    }
    virtual void dump() {
        std::cout << "(FCN: "; 
        std::cout << " )";
    }
};

class AssertExprCell : public CallExprCell {
private:
    bool assertResult;
public:
    AssertExprCell(SymbolPtr sarg1, bool assertResult, Instruction *i) 
    : CallExprCell(NULL, sarg1, i), assertResult(assertResult) {
        assert(i && "Expecting an instruction!");
        ExprPtr e = sarg1->convertToExpr();
        if (i->getType()->isIntegerTy(1)) {
            this->expr = e;
            this->notExpr = Expression::mkNot(e);
        } else {
            ExprPtr oneExpr = Expression::mkSInt32Num(1);
            this->expr      = Expression::mkEq(e, oneExpr);
            this->notExpr   = Expression::mkDiseq(e, oneExpr);
        }
    }
    bool getResult() { return assertResult; }
    virtual bool isAssert() { return true; } 
    virtual void dump() {
        std::cout << "(AC: ";
        expr->dump();
        std::cout << " )";
        if (assertResult) {
            std::cout << " <T>";
        } else {
            std::cout << " <F>";
        }
    }
    
};

class AssumeExprCell : public CallExprCell {
private:
    bool assumeResult;
public:
    AssumeExprCell(SymbolPtr sarg1, bool assumeResult, Instruction *i) 
    : CallExprCell(NULL, sarg1, i), assumeResult(assumeResult) { 
        this->expr =  sarg1->convertToExpr();
        this->notExpr = Expression::mkNot(this->expr);
    }
    bool getResult() { return assumeResult; }
    virtual bool isAssume() { return true; } 
    virtual void dump() {
        std::cout << "(AS: ";
        expr->dump();
        std::cout << " )";
        if (assumeResult) {
            std::cout << " <T>";
        } else {
            std::cout << " <F>";
        }
    }
    
};

#endif
