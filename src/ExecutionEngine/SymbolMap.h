/**
 * \file LoopInfoPass.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   15 February 2015
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


/**
 * \class SymbolMap
 *
 * This class map LLVM values to symbols.
 * This class is a part of the concolic execution.
 */
class SymbolMap {
    
private:
    /**
     * Map LLVM values to symbols.
     */
    std::map<Value*, SymbolPtr> memory;

public:
    SymbolMap() { }
    ~SymbolMap() {
        memory.clear();
    }

    /**
     * Create and add a new symbol from an LLVM input value.
     *
     * \param v An LLVM value (function argument) to map.
     * \return A new symbol representating the value \p v.
     */
    SymbolPtr addInput(Value *v);
    /**
     * Create and add a new symbol from an LLVM value.
     *
     * \param v An LLVM value to map.
     * \return A new symbol representating the value \p v.
     */
    SymbolPtr create(Value *v);
    /**
     * Return the symbol associated to the LLVM value \p v.
     *
     * \param v An LLVM value.
     * \return The symbol that represents the value \p v.
     */
    SymbolPtr get(Value *v);
    /**
     * Remove the entry <s,v> from the map, with \p s the symbol of \p v.
     * 
     * \param v An LLVM value.
     */
    void remove(Value *v);
    /**
     * Replace the entry <s,v> in the map by <s,v1>.
     * 
     * \param v An LLVM value.
     * \param v1 An LLVM value.
     */
    void update(Value *v, Value *v1);
    /**
     * Replace the entry <s0,v> in the map by <s,v>.
     * 
     * \param v An LLVM value.
     * \param s A symbol.
     */
    void update(Value *v, SymbolPtr s);
    /**
     * Return true if there exist an entry for \p v.
     */
    bool contains(Value *v);
    /**
     * Remove all entries in the map. 
     */
    void clear();
    /**
     * Dump to the standard output the map entries.
     */
    void dump();
    
    /**
     * Create a new symbol s(v) representating a binary operation (add, sub, ...).
     * s(v)= s1 op s2.
     *
     * \param v An LLVM value (binary instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param s2 A symbol (second argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createBinaryOp(Value *v, SymbolPtr s1, SymbolPtr s2);
    /**
     * Create a new symbol s(v) representating a binary operation (add, sub, ...).
     * s(v)= s1 op s(v2).
     *
     * \param v An LLVM value (binary instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param v2 An LLVM value (second argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createBinaryOp(Value *v, SymbolPtr s1, Value *v2);
    /**
     * Create a new symbol s(v) representating a binary operation (add, sub, ...).
     * s(v)= s(v1) op s2.
     *
     * \param v An LLVM value (binary instruction).
     * \param v1 An LLVM value (first argument of the operation).
     * \param s2 A symbol (second argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createBinaryOp(Value *v, Value *v1,  SymbolPtr s2);
    /**
     * Create a new symbol s(v) representating a select operation.
     * s(v)= s1 ? s2 : s3
     *
     * \param v An LLVM value (select instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param s2 A symbol (second argument of the operation).
     * \param s3 A symbol (third argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, SymbolPtr s2, SymbolPtr s3);
    /**
     * Create a new symbol s(v) representating a select operation.
     * s(v)= s1 ? s(v2) : s3
     *
     * \param v An LLVM value (select instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param v2 An LLVM value (second argument of the operation).
     * \param s3 A symbol (third argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, Value *v2, SymbolPtr s3);
    /**
     * Create a new symbol s(v) representating a select operation.
     * s(v)= s1 ? s2 : s(v3)
     *
     * \param v An LLVM value (select instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param s2 A symbol (second argument of the operation).
     * \param v3 An LLVM value (third argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, SymbolPtr s2, Value *v3);
    /**
     * Create a new symbol s(v) representating a select operation.
     * s(v)= s1 ? s(v2) : s(v3)
     *
     * \param v An LLVM value (select instruction).
     * \param s1 A symbol (first argument of the operation).
     * \param v2 An LLVM value (second argument of the operation).
     * \param v3 An LLVM value (third argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createSelectOp(Value *v, SymbolPtr v1, Value *v2, Value *v3);
    /**
     * Create a new symbol s(v) representating a cast operation.
     * s(v)= cast s1
     *
     * \param v An LLVM value (cast instruction).
     * \param s1 A symbol (argument of the operation).
     * \return A new symbol.
     */
    SymbolPtr createCastOp(Value *v, SymbolPtr s1);
    /**
     * Create a new symbol p(ptr) representating a get pointer operation.
     * p(ptr) = gep *a, s1
     * Limitation: only support integer values (*a cannot be symbolic)
     *
     * \param ptr An LLVM value (pointer).
     * \param a An LLVM value (base address).
     * \param s1 A symbol (address offset).
     * \return A new symbol.
     */
    SymbolPtr createGepOp(Value *ptr, Value *a, SymbolPtr s1);
    
};

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

#endif // _SYMBOLMAP_H
