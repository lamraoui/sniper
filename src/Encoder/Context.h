/**
 * Context.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/04/11
 * @copyright : NII 2014
 */

#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <string>
#include <iostream>

#include "llvm/IR/LLVMContext.h"

#include "Options.h"
#include "Logic/Formula.h"
#include "Frontend/LocalVariables.h"
#include "Frontend/LoopInfoPass.h"

using namespace llvm;


// TODO : unordered_map
typedef std::map<Value*, ExprPtr> val2expr_map;
typedef std::map<std::string, ExprPtr> str2trans_map;
typedef std::map<std::string, ExprPtr> name2expr_map;

//============================================================================
class Context {

private:
    LocalVariables *locVar;
    Options *options;
    bool is64Bit;
    unsigned phiCounter;
    unsigned memPtr, memID;
    unsigned argvMaxIndex;
    
    // Value
    val2expr_map val2expr;
    val2expr_map val2num;
    val2expr_map val2lbl;
    
    str2trans_map str2trans;
    name2expr_map name2expr;
    
    // GlobalVariable 
    std::map<Value*, int> gv2v;
    
    // Pointers ID
    std::map<Value*, int> ptr2id;
    std::map<BasicBlock*, int> bb2id;
    
    std::map<unsigned,bool> assertCall2lines;
    
    // TCAS
    std::string assertVarName;
    unsigned arrayCheckId;
    std::vector<ExprPtr> arrayCheckExprs;
    
public:
    Context(LocalVariables *_locVar, Options *_options);
    ~Context() { }
    
    void    propagatePointers(BasicBlock *bb, BasicBlock *predbb);
    ExprPtr propagatePointers(BasicBlock *bb);
    ExprPtr newVariable(Value *val, BasicBlock *bb = 0);
    ExprPtr newVariableWithType(Value *val, Value *baseType);
    ExprPtr newTransition(BasicBlock *pred, BasicBlock *cur);
    ExprPtr getTransition(BasicBlock *pred, BasicBlock *cur);
    ExprPtr getVariable(Value *val);
    ExprPtr getCondVariable(Value *val);
    ExprPtr getLocalVariable(Instruction *i, Value *arg, int pos);
    
    void setAssertCallLine(unsigned line) { assertCall2lines[line] = true; }
    bool isAssertCall(unsigned line) { return assertCall2lines[line]; }
    void setArgvMaxIndex(unsigned i) { argvMaxIndex = i; }
    unsigned getArgvMaxIndex() { return argvMaxIndex; }
    // Memory utils warper
    unsigned getPtrId(AllocaInst *a) {
       return ptr2id[a]; 
    }
    void updateMemId(StoreInst *s) {
        memID++; 
        bb2id[s->getParent()] = memID;
    }
    unsigned getMemId(BasicBlock *bb) { return bb2id[bb]; }
    unsigned getCurrentMemId() { return memID; }
    void addPtrId(AllocaInst *alloca, uint64_t size) {
        ptr2id[alloca] = memPtr;
        memPtr = memPtr + size;
    }
    void setTCASAssertVarName(std::string name) { assertVarName = name; }
    std::string getTCASAssertVarName() { return assertVarName; }
    
private:
    void dump();

};
//============================================================================


#endif