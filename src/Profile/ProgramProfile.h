/**
 * ProgramProfile.h
 *
 * Save the information related to executions of a program:
 * - inputs that trigger the execution
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
    // The set bugFreeBlocks is only accessible
    // when bugFreeBlocksComputed is true.
    std::set<BasicBlock*> bugFreeBlocks;
    bool bugFreeBlocksComputed;
    
public:
    ProgramProfile(Function *_targetFun) 
    : targetFun(_targetFun), bugFreeBlocksComputed(false) {  }    
    ~ProgramProfile() { }
    
    // Traces
    void addProgramTrace(ProgramTrace *e);
    bool hasFailingProgramTraces();
    std::vector<ProgramTrace*> getProgramTraces();
    std::vector<ProgramTrace*> getFailingProgramTraces();
    std::vector<ProgramTrace*> getSuccessfulProgramTraces();
    std::vector<ProgramTrace*> getUnkownProgramTraces();
    
    // For constructing HFTF
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

#endif
