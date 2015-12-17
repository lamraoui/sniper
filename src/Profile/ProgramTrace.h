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



class ProgramTrace {

private:
    static unsigned ID;
    const unsigned myID;
    Function *targetFun;
    std::set<BasicBlock*> executedBlocks;
    VariablesPtr inputVars;
    Value *expectedOutput; // oracle
    AssertResult type;
    
public:
    ProgramTrace(Function *_targetFun)
    : myID(ID++), targetFun(_targetFun), type(UNKNOW) {
        inputVars = std::make_shared<Variables>();
        expectedOutput = NULL;
    }
    
    ProgramTrace(Function *_targetFun, AssertResult _type)
    : myID(ID++), targetFun(_targetFun), type(_type) {
        inputVars = std::make_shared<Variables>();
        expectedOutput = NULL;
    }
    
    ProgramTrace(Function *_targetFun, std::vector<Value*> _inputs,
                 AssertResult _type)
    : myID(ID++), targetFun(_targetFun), type(_type) {
        assert(_targetFun->arg_size()==_inputs.size() &&
               "Wrong execution trace!");
        unsigned i = 0;
        Function::arg_iterator ait;
        for (ait = targetFun->arg_begin(); ait != targetFun->arg_end(); ++ait) {
            Value *origin = ait;
            Value *concrete = _inputs[i++];
            addProgramInput(origin, concrete);
        }
        expectedOutput = NULL;
    }
    
    ~ProgramTrace() {  }
    
    // Input values
    void            addProgramInput(Value *origin, Value *val);
    void            addProgramInput(Value *origin, int val);
    void            addProgramInput(InputVarTracePtr ivt);
    //void          addProgramInput(std::string argvValue);
    ExprPtr         getProgramInputsFormula(Formula *formula);
    VariablesPtr    getInputVariables() { return inputVars; }
    void            dumpProgramInputs() ;
    
    // Block profile
    void setExecutedBlocks(std::vector<BasicBlock*> &bb);
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
    
    // Miscellaneous
    unsigned getID() { return myID;}
    
    bool operator<(const ProgramTrace &other) const { 
        return (inputVars<other.inputVars);
    }
    bool operator==(const ProgramTrace &other) const { 
        return (inputVars==other.inputVars);
    }
    
    void dump();
    
};
//============================================================================

#endif
