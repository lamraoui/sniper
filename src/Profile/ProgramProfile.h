/**
 * ProgramProfile.h
 *
 * Save the information related to executions of a program:
 * - inputs that trigger the execution, 
 * - function summaries,
 * - non-linear operation summaries
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/11/15
 * @copyright : NII 2013
 */


#ifndef _PROGRAMPROFILE_H
#define _PROGRAMPROFILE_H

#include <set>
#include <vector>
#include <iostream>
#include <memory>

#include "llvm/DebugInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "Options.h"
#include "Profile/ProgramTrace.h"

using namespace llvm;

class NLOperationSummary;
class FunctionSummary;


//============================================================================
class ProgramProfile {

private:
    Function *targetFun;
    std::map<std::string, FunctionSummary*> funSummariesMap;
    std::map<DILocation, NLOperationSummary*> nlOpSummariesMap;
    std::vector<ProgramTrace*> traces;
    //std::set<BasicBlock*> failingBlocks;
    //std::set<BasicBlock*> successfulBlocks;
    //std::set<BasicBlock*> unknownBlocks;
    std::set<BasicBlock*> bugFreeBlocks; // only accessible when bugFreeBlocksComputed=true
    bool bugFreeBlocksComputed;
    
public:
    ProgramProfile(Function *_targetFun) 
    : targetFun(_targetFun), bugFreeBlocksComputed(false) {  }    
    ~ProgramProfile() { }
    
    // Traces
    void addProgramTrace(ProgramTrace *e);
    bool hasFailingProgramTraces();
    std::vector<ProgramTrace*> getProgramTraces();
    std::vector<ProgramTrace*> getFailingProgramTraces(Options *o = NULL);
    std::vector<ProgramTrace*> getSuccessfulProgramTraces();
    std::vector<ProgramTrace*> getUnkownProgramTraces();
    void removeNFirstFailingProgramTraces(unsigned n);
    void removeNFirstSuccessfulProgramTraces(unsigned n);
    
    // Summaries
    void addFunSummary(Function *f, Value *input, Value *output);
    void addNLOpSummary(DILocation loc, Value *y, Value *z);
    FunctionSummary* getFunSummary(Function *f);
    NLOperationSummary* getNLOpSummary(BinaryOperator *bo); 
    
    // For constructing PTF
    void addFailingBlocks(std::vector<BasicBlock*> &bb);
    void addSuccessfulBlocks(std::vector<BasicBlock*> &bb);
    void addUnknowBlocks(std::vector<BasicBlock*> &bb);
    void computeBugFreeBlocks(Options *o); 
    bool isBugFreeBlock(BasicBlock *bb);
    std::set<BasicBlock*> getFailingRunBB();
    std::set<BasicBlock*> getSuccessfulRunBB();
    std::set<BasicBlock*> getOtherRunBB();
    std::set<BasicBlock*> getBugFreeBlocks();
    
    void dump(); 
    
private:
    void addSuccProgramTrace(ProgramTrace *e);
    void addFailProgramTrace(ProgramTrace *e);
    void addOtherProgramTrace(ProgramTrace *e);
    
};

//============================================================================
class NLOperationSummary {
    
private:
    DILocation loc; // dbg info of the non-linear operation 
    std::vector<std::pair<Value*,Value*> > values; // <y,z> of x = y op z
    
public:
    NLOperationSummary(DILocation _loc);
    ~NLOperationSummary();
    void addValues(Value *y, Value *z);
    std::vector<std::pair<Value*,Value*> > getValues();
    void dump();
};

//============================================================================
typedef struct io {
    std::vector<Value*> inputs;
    Value *output;
} io_t;

class FunctionSummary {
    
private:
    Function *foo;
    std::vector<io_t> inOuts;
    
public:
    FunctionSummary(Function *_foo);
    ~FunctionSummary();
    void addIO(Value *input, Value *output);
    std::vector<io_t> getInputOutputs();
    void dump();
};

//============================================================================
#endif
