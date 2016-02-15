/**
 * \file LoopInfoPass.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   15 February 2015
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

/**
 * \class LoopInfoPass
 *
 * This class is an LLVM pass that processes loops in the target function.
 */
class LoopInfoPass : public FunctionPass {
    
    /**
     * LLVM pass ID.
     */
    static char ID;

    /**
     * Set required information for the pass
     * (see LLVM Pass documentation for details).
     */
    virtual void getAnalysisUsage(AnalysisUsage &Info) const {
        Info.addRequired<LoopInfo>();
        Info.addPreserved<LoopInfo>();
    }
    
private:
    /**
     * Target LLVM module.
     */
    Module *llvmMod;
    /**
     * True if debug message are shown, false otherwise.
     */
    bool dbgMsg;
    /**
     * Map LLVM branch instructions to loop header basic block.
     * These branch instructions jump to the loop headers. 
     */
    std::map<BranchInst*, BasicBlock*> br2header;
    
public:
    LoopInfoPass(Module *_llvmMod, bool _dbgMsg) 
    : llvmMod(_llvmMod), dbgMsg(_dbgMsg), FunctionPass(ID) { }
    ~LoopInfoPass() { }

    /**
     * Return an instance of this class.
     * 
     * \param llvmMod The target LLVM module.
     * \param dbgMsg True if debug message are shown, false otherwise.
     * \return An instance of this LoopInfoPass. 
     */
    Pass *createLoopInfoPass(Module *llvmMod, bool dbgMsg);
    /**
     * LLVM executes this function on each function 
     * of the target LLVM module.
     *
     * \param F A LLVM function to be processed by the pass. 
     * \return true if the pass modified the the function \p F.
     */
    virtual bool runOnFunction(Function &F);
    /**
     * Return the latch basic block associated to the given branch instruction.
     * The branch instruction \p br jump to either a basic block B1 or B2. 
     * This function returns either B1 or B2.
     * For recall, a latch basic block is a basic block that unconditionally jumps 
     * to a the loop header basic block. 
     *
     * \param br An LLVM branch instruction.
     * \return A latch basic block.
     */
    BasicBlock* getLoopLatch(BranchInst *br);
    /**
     * Returns the basic block's name of the latch basic block 
     * associated with the given branch instruction 
     * (see getLoopLatch function for details).
     * 
     * \param br An LLVM branch instruction.
     * \return  A latch basic block's name.
     */
    std::string getLoopLatchName(BranchInst *br);
    /**
     * Return true if \p bb is a latch basic block.
     */
    bool isLoopLatch(BasicBlock *bb);
    /**
     * Return all LLVM branch instruction associated 
     * with latch basic blocks.
     */
    std::vector<BranchInst*> getLoopLatchBrs();
    /**
     * Return true if the target LLVM module contains loops. 
     */
    bool hasLoops();
    
private:
    /**
     * Collect and save latch basic blocks in the loop \p l.
     * TODO: remove back edge and add sniper_assert(false) function 
     * to show that the loop \p l was not unroll enough in case the 
     * assert is reached.
     */
    void assertLoop(Loop *l);
};

#endif // LOOPINFOPASS_H