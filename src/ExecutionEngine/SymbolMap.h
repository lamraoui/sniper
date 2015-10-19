/**
 * SymbolMap.h
 *
 * 
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
 */


#ifndef _SYMBOLMAP_H
#define _SYMBOLMAP_H

#include <string>
#include <iostream>
#include <map>
#include <memory>

#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"

#include "Symbol.h"

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


//============================================================================
class SymbolMap {
    
private:
    std::map<Value*, SymbolPtr> memory;
public:
    SymbolMap();
    ~SymbolMap();
    SymbolPtr addInput(Value *v);
    SymbolPtr create(Value *v);
    SymbolPtr get(Value *v);
    void remove(Value *v);
    void update(Value *v, Value *v1);
    void update(Value *v, SymbolPtr s);
    bool contains(Value *v);
    void clear();
    void dump();
    
    SymbolPtr createBinaryOp(Value *v, SymbolPtr s1, SymbolPtr s2);
    SymbolPtr createBinaryOp(Value *v, SymbolPtr s1, Value *v2);
    SymbolPtr createBinaryOp(Value *v, Value *v1,  SymbolPtr s2);
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, SymbolPtr s2, SymbolPtr s3);
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, Value *v2, SymbolPtr s3);
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, SymbolPtr s2, Value *v3);
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, Value *v2, Value *v3);
    SymbolPtr createCastOp(Value *v, SymbolPtr s1);
    SymbolPtr createGepOp(Value *ptr, Value *a, SymbolPtr s1);
    
};
//============================================================================



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
//============================================================================


//============================================================================
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
//============================================================================


//============================================================================
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
//============================================================================


//============================================================================
class AssertExprCell : public CallExprCell {
private:
    bool assertResult;
public:
    AssertExprCell(SymbolPtr sarg1, bool assertResult, Instruction *i) 
    : CallExprCell(NULL, sarg1, i), assertResult(assertResult) { 
        ExprPtr oneExpr = Expression::mkSInt32Num(1);
        ExprPtr e       = sarg1->convertToExpr();
        this->expr      = Expression::mkEq(e, oneExpr);
        this->notExpr   = Expression::mkDiseq(e, oneExpr);
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
//============================================================================


//============================================================================
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
//============================================================================


#endif
