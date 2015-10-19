/**
 * LoopInfoPass.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/07/31
 * @copyright : NII 2013
 */

#ifndef LOOPINFOPASS_H
#define LOOPINFOPASS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <llvm/IR/LLVMContext.h>
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/CodeMetrics.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Transforms/Utils/SimplifyIndVar.h"
#include "llvm/Analysis/TargetTransformInfo.h"

#include "Utils/utils.h"

using namespace llvm;


//============================================================================
class LoopInfoPass : public FunctionPass {
    
    static char ID;
    virtual void getAnalysisUsage(AnalysisUsage &Info) const {
        Info.addRequired<LoopInfo>();
        Info.addPreserved<LoopInfo>();
    }
    
private:
    Module *llvmMod;
    bool dbgMsg;
    Function *assertFun;
    std::map<BranchInst*, BasicBlock*> br2header;
    
public:
    LoopInfoPass(Module *_llvmMod, bool _dbgMsg);
    ~LoopInfoPass() { }
    Pass *createLoopInfoPass(Module *llvmMod, bool dbgMsg);
    virtual bool runOnFunction(Function &F);
    BasicBlock* getLoopLatch(BranchInst *br);
    std::string getLoopLatchName(BranchInst *br);
    bool isLoopLatch(BasicBlock *bb);
    std::vector<BranchInst*> getLoopLatchBrs();
    bool hasLoops();
    
private:
    void assertLoop(Loop *l);
};
//============================================================================

#endif