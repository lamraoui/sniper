/**
 * ProgramTrace.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/04/02
 * @copyright : NII 2014
 */


#ifndef _PROGRAMTRACE_H
#define _PROGRAMTRACE_H

#include <iostream>
#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"

#include "Logic/Formula.h"
#include "Logic/Expression.h"

using namespace llvm;

// Program assert outcome
enum AssertResult {
    SUCCESS,
    FAIL,
    UNKNOW
};


class Variables;
typedef std::shared_ptr<Variables> VariablesPtr;
class InputVariableTrace;
typedef std::shared_ptr<InputVariableTrace> InputVarTracePtr;


//============================================================================
class InputVariableTrace {
private:
    Value *concrete;
    std::string name;
private:
    InputVariableTrace(Value *o) { 
        name = o->getName().str();
        assert(name.length()>0 && "Empty string!");
    }
public:
    InputVariableTrace(Value *o, Value *c) : InputVariableTrace(o) { 
        concrete = c;
    }
    InputVariableTrace(Value *o, int val) : InputVariableTrace(o) { 
        LLVMContext &context = getGlobalContext();
        IRBuilder<> IRB(context);
        concrete = IRB.getInt32(val);
    }
    ~InputVariableTrace() { 
        //delete concrete; // BUG
    }
    std::string getName() {
        return name;
    }
    int getInt32() const { 
        ConstantInt *ci = dyn_cast<ConstantInt>(concrete);
        assert(ci && "No concrete value for variable!");
        return (int) ci->getSExtValue();
    }
    Value* getConcrete() { return concrete; }
    void dump() {
        ConstantInt *ci = dyn_cast<ConstantInt>(concrete);
        assert(ci && "no concrete value for variable!");
        std::cout << ci->getSExtValue();
    }
    /*bool operator<(const InputVariableTrace &other) const { 
        return (name<other.name && getInt32()<other.getInt32());
    }
    bool operator==(const InputVariableTrace &other) const { 
        return (name==other.name && getInt32()==other.getInt32());
    }
    bool operator!=(const InputVariableTrace &other) const { 
        return (name!=other.name || getInt32()!=other.getInt32());
    }*/
    bool isEqualTo(InputVarTracePtr &other) const { 
        return (name==other->getName() && getInt32()==other->getInt32());
    }
    bool isValueEqualTo(InputVarTracePtr &other) const { 
        return (getInt32()==other->getInt32());
    }
};

class Variables {
private:
    std::vector<InputVarTracePtr> vars;
public:
    Variables() { }
    ~Variables() { }
    void add(Value *o, Value *c) {
        InputVarTracePtr ptr = std::make_shared<InputVariableTrace>(o, c);
        add(ptr);
    }
    void add(Value *o, int val) {
        InputVarTracePtr ptr = std::make_shared<InputVariableTrace>(o, val);
        add(ptr);
    }
    void add(InputVarTracePtr v) {
        const bool isIn = std::find(vars.begin(), vars.end(), v)!=vars.end();
        if (!isIn) {
            vars.push_back(v);
        }
    }
    unsigned size() { 
        return vars.size(); 
    }
    bool empty() { 
        return vars.empty(); 
    }
    // TEMPORARY //
    std::vector<InputVarTracePtr> getVector() {
        return vars;
    }
    //  ------- //
    std::vector<Value*> getValues() {
        std::vector<Value*> vals;
        std::vector<InputVarTracePtr>::iterator it;
        for(it=vars.begin(); it!=vars.end(); ++it) {
            InputVarTracePtr v = *it;
            vals.push_back(v->getConcrete());
        }
        return vals;
    }
    void dump() {
        std::vector<InputVarTracePtr>::iterator it;
        for(it=vars.begin(); it!=vars.end(); ++it) {
            InputVarTracePtr v = *it;
            std::cout << /*v.name << "=" <<*/ v->getInt32();
            if (it+1!=vars.end()) {
                std::cout << " ";
            }
        }
    }
    bool isValueEqualTo(VariablesPtr &other) {
        if (vars.size()!=other->size()) {
            return false;
        }
        std::vector<InputVarTracePtr> vars2 = other->getVector();
        for (std::size_t i=0; i<vars.size(); ++i) {
            if (!vars[i]->isValueEqualTo(vars2[i])) {
                return false;
            }
        }
        return true;   
    }
    /*bool operator==(const Variables &other) const { 
        std::cout << "COMP!!!\n";
        if (vars.size()!=other.vars.size()) {
            return false;
        }
        for (int i=0; i<vars.size(); ++i) {
            if (vars[i]!=other.vars[i]) {
                return false;
            }
        }
        return true;
    }*/
};

class InstructionTrace {
protected:
    Instruction *inst;
    Value *resVal; // concrete value
    std::vector<Value*> argVals; // concrete values
public:
    InstructionTrace(Instruction *i) : inst(i) {
        resVal = NULL;
    }
    InstructionTrace(Instruction *i, Value *c) : inst(i) {
        if (i->isTerminator()) {
            resVal = NULL;
            argVals.push_back(c);
        }
        else if (isa<PHINode>(i)) {
            resVal = c;
            argVals.push_back(c);
        }
        else if (isa<StoreInst>(i) || isa<LoadInst>(i)) {
            argVals.push_back(c);
        }
        else {
            i->dump();
            llvm_unreachable("InstructionTrace");
        }
    }
    InstructionTrace(Instruction *i, Value *c1, Value *c2) : inst(i) {
        assert(i->mayReturn() && i->getNumOperands()==2 && "InstructionTrace");
        resVal = emulateInst(i, c1, c2);
        argVals.push_back(c1);
        argVals.push_back(c2);
    }
    InstructionTrace(Instruction *i, Value *c1, Value *c2, Value *c3) : inst(i) {
        assert(i->mayReturn() && i->getNumOperands()==3 && "InstructionTrace");
        resVal = emulateInst(i, c1, c2, c3);
        argVals.push_back(c1);
        argVals.push_back(c2);
        argVals.push_back(c3);
    }
    ~InstructionTrace() {
        /* BUG
        delete resVal;
        while (!argVals.empty()) {
            Value *v = argVals.back();
            argVals.pop_back();
            delete v;
        }*/
    }
    Instruction* getInstruction() { return inst; }
    void setInstruction(Instruction *i) { inst = i; }
    Value* getConcrete(Value *v);
    void dump();
    
private:
    int getInt32(Value *v);
    static int    emulateInst(Instruction *i, int val1, int val2);
    static int    emulateInst(Instruction *i, int val1, int val2, int val3);
    static Value* emulateInst(Instruction *i, Value *val1, Value *val2);
    static Value* emulateInst(Instruction *i, Value *val1, Value *val2,
                              Value *val3);
    
};

typedef std::shared_ptr<InstructionTrace> InstTracePtr;

class ProgramTrace {

private:
    static unsigned ID;
    const unsigned myID;
    Function *targetFun;
    std::vector<InstTracePtr> instructionTraces;
    std::set<BasicBlock*> executedBlocks;
    //std::vector<std::string> argvs;
    VariablesPtr inputVars;
    Value *expectedOutput; // oracle
    AssertResult type;
    
public:
    ProgramTrace(Function *_targetFun);
    ProgramTrace(Function *_targetFun, AssertResult _type);
    ProgramTrace(Function *_targetFun, std::vector<Value*> _inputs, AssertResult _type);
    ~ProgramTrace() {  }
    
    // Input values
    void    addProgramInput(Value *origin, Value *val);
    void    addProgramInput(Value *origin, int val);
    void    addProgramInput(InputVarTracePtr ivt);
    void    dumpProgramInputs() ;
    ExprPtr getProgramInputsFormula(Formula *formula);
    VariablesPtr getInputVariables() { return inputVars; }
    //void     addProgramInput(std::string argvValue);
    
    // Block profile
    void setExecutedBlocks(std::vector<BasicBlock*> &bb);
    
    // Instruction profiles
    void add(Instruction *i);
    void add(Instruction *i, Value *v);
    void add(Instruction *i, Value *v1, Value *v2);
    void add(Instruction *i, Value *v1, Value *v2, Value *v3);
    std::vector<InstTracePtr> getInstructionTraces();
    Value* getConcreteValue(Value *v);
    std::set<BasicBlock*> getExecutedBB();
    
    // Trace type (outcome)
    bool isSuccessful()  { return (type==SUCCESS); }
    bool isFailing()     { return (type==FAIL);    }
    bool isUnknow()      { return (type==UNKNOW);  }
    void setFailing()    { type = FAIL;    }
    void setSuccessful() { type = SUCCESS; }
    void setUnknow()     { type = SUCCESS; }
    
    // Oracle
    void setExpectedOutput(Value *o) { expectedOutput = o; }
    Value *getExpectedOutput() { return expectedOutput; }
    
    // Misealous
    void clearInstructionTraces() { instructionTraces.clear(); }
    unsigned getID() { return myID;}
    
    bool operator<(const ProgramTrace &other) const { 
        return (inputVars<other.inputVars);
    }
    bool operator==(const ProgramTrace &other) const { 
        return (inputVars==other.inputVars);
    }
    
    void dump() {
        dumpProgramInputs();
        std::string st = "unknow";
        if (type==FAIL) {
            st = "failing";
        } else if (type==SUCCESS) {
            st = "successful";
        }
        std::cout << " (ID=" << myID << ")  [" << st << "]\n";
        std::vector<InstTracePtr>::const_iterator cit;
        for (cit=instructionTraces.begin(); cit!=instructionTraces.end(); ++cit) {
            InstTracePtr i = *cit;
            /*i->dump();
            std::cout << " (" << i->getInstruction()->getParent()->getName().str() << ")";
            std::cout << std::endl;
            */
            if (i->getInstruction()->getParent()->getName().str()=="bb45" ||
                i->getInstruction()->getParent()->getName().str()=="bb43") {
                i->dump();
                std::cout << " (" << i->getInstruction()->getParent()->getName().str() << ")";
                std::cout << std::endl;
            }
        }
    }
    
};
//============================================================================

#endif
