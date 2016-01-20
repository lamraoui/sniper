/**
 * \file ProgramTrace.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   20 January 2015
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

/** 
 * A program post-condition outcome of an execution trace.
 */
enum AssertResult {
    SUCCESS, /*!< Post-condition was satisfied */
    FAIL,    /*!< Post-conditions was violated */
    UNKNOW   /*!< Post-condition's outcome is undetermined */
};

class Variables;
class InputVariableTrace;

typedef std::shared_ptr<Variables> VariablesPtr;
typedef std::shared_ptr<InputVariableTrace> InputVarTracePtr;

/**
 * \class ProgramTrace
 *
 * \brief Execution trace.
 *
 * An execution trace holds the input values that trigger the execution,
 * the executed LLVM basicblocks, and the expected program output (oracle).
 * When using a post-condition formula with \a sniper_assert, no oracle is used.
 */
class ProgramTrace {
    
private:
    /**
     * Unique ID number to be assigned to each expression.
     */
    static unsigned ID;
    const unsigned myID;
    /**
     * LLVM function from which this trace was generated.
     */
    Function *targetFun;
    /**
     * Set of LLVM basicblocks that were executed in the 
     * associated to this trace.
     */
    std::set<BasicBlock*> executedBlocks;
    /**
     * Program input values used to trigger this trace.
     */
    VariablesPtr inputVars;
    /**
     * Expected program output for the given input values (oracle).
     */
    Value *expectedOutput;
    /**
     * Type of trace: failing, successful, or not determined.
     */
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
    
    // ==== Input values ====
    
    /**
     * Add a program input value.
     *
     * \param origin Value in the LLVM function.
     * \param val Contant LLVM value.
     */
    void addProgramInput(Value *origin, Value *val);
    /**
     * Add a program input value.
     *
     * \param origin Value in the LLVM function.
     * \param val Contant integer value.
     */
    void addProgramInput(Value *origin, int val);
    /**
     * Add a program input value.
     */
    void addProgramInput(InputVarTracePtr ivt);
    /**
     * Return a formula that represents input values with equalities.
     * \f$(input_1 = val_1) \land ... \land (input_n = val_n)\f$
     */
    ExprPtr getProgramInputsFormula(Formula *formula);
    /**
     * Return input variables.
     */
    VariablesPtr getInputVariables() {
        return inputVars;
    }
    /**
     * Dump to the standard ouput the program input values.
     */
    void dumpProgramInputs() ;
    
    // ==== Block profile ====
    
    /**
     * Assign executed LLVM basicblocks for this trace.
     */
    void setExecutedBlocks(std::vector<BasicBlock*> &bb);
    /**
     * Return the executed LLVM basicblocks of this trace.
     */
    std::set<BasicBlock*> getExecutedBB();
    
    // ==== Trace type (outcome) ====
    
    /**
     * Return \a true if the post-condition was satified
     * in the execution of this trace, false otherwise.
     */
    bool isSuccessful() {
        return (type==SUCCESS);
    }
    /**
     * Return \a true if the post-condition was violated
     * in the execution of this trace, false otherwise.
     */
    bool isFailing() {
        return (type==FAIL);
    }
    /**
     * Return \a true if the post-condition outcome is undeterminated
     * in the execution of this trace, false otherwise.
     */
    bool isUnknow() {
        return (type==UNKNOW);
    }
    /**
     * Set this trace as failing (post-condition violated).
     */
    void setFailing() {
        type = FAIL;
    }
    /**
     * Set this trace as successful (post-condition satisfied).
     */
    void setSuccessful() {
        type = SUCCESS;
    }
    /**
     * Set this trace as nor successful, nor failing.
     */
    void setUnknow() {
        type = UNKNOW; // SUCCESS
    }
    
    // ==== Oracle ====
    
    /**
     * Set expected program output (oracle).
     */
    void setExpectedOutput(Value *o) {
        expectedOutput = o;
    }
    /**
     * Get expected program output (oracle).
     */
    Value *getExpectedOutput() {
        return expectedOutput;
    }
    
    // ==== Miscellaneous ====
    
    /**
     * Get the unique ID number of this trace.
     */
    unsigned getID() {
        return myID;
    }
    bool operator<(const ProgramTrace &other) const {
        return (inputVars<other.inputVars);
    }
    bool operator==(const ProgramTrace &other) const {
        return (inputVars==other.inputVars);
    }
    
    /**
     * Dump to the standard ouput the information of this trace.
     */
    void dump();
    
};

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

#endif // _PROGRAMTRACE_H
