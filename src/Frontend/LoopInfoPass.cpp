/**
 * LoopInfoPass.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2011/05/31
 * @copyright : NII 2012
 */

#include "LoopInfoPass.h"


char LoopInfoPass::ID = 0;


LoopInfoPass::LoopInfoPass(Module *_llvmMod, bool _dbgMsg) : FunctionPass(ID) {
    this->llvmMod = _llvmMod;
    this->dbgMsg = _dbgMsg;
}


Pass *LoopInfoPass::createLoopInfoPass(Module *llvmMod, bool dbgMsg) { 
    return new LoopInfoPass(llvmMod, dbgMsg); 
}


void LoopInfoPass::assertLoop(Loop *l) {
    
    BranchInst *exitingBlockBr = NULL;
    // If getExitingBlocks would return exactly one block,
    // return that block. Otherwise return null.
    BasicBlock *exitingBlock = l->getExitingBlock();
    if (exitingBlock) {
        Instruction *termInst = exitingBlock->getTerminator();
        exitingBlockBr = dyn_cast<BranchInst>(termInst);
    } else {
        // If there is a single latch block for this loop, return it.
        // A latch block is a block that contains a branch back to the header.
        BasicBlock *latchBlock = l->getLoopLatch();
        if (latchBlock) {
            Instruction *termInst = latchBlock->getTerminator();
            exitingBlockBr = dyn_cast<BranchInst>(termInst);
        } 
        
    }
    bool success = false;
    if (exitingBlockBr && exitingBlockBr->isConditional()) {
        // Look for the successor blocks that are in the loop
        for (unsigned i=0; i<exitingBlockBr->getNumSuccessors(); ++i) {
            BasicBlock *succ = exitingBlockBr->getSuccessor(i);
            if (l->contains(succ)) {
                br2header[exitingBlockBr] = succ;
                success = true;
                break;
            }
        }
    }
    if (!success) {
        std::cout << "exit : unable to handle loop:\n";
        //l->dump();
        exit(1);
    }
}

// Does not work on cloned Modules
BasicBlock* LoopInfoPass::getLoopLatch(BranchInst *br) {
    // Check if br comes from the same module (not cloned)
    Module *brMod  = br->getParent()->getParent()->getParent();
    if (brMod!=llvmMod) {
        std::cout << "error : br come from a different module\n";
        exit(1);
    }
    std::map<BranchInst*,BasicBlock*>::iterator it = br2header.find(br);
    if(it!=br2header.end()) {
        return it->second;
    } else {
        return NULL;
    }
}

// Work even for cloned Modules
std::string LoopInfoPass::getLoopLatchName(BranchInst *br) {
    if (!br) {
        return "";
    }
    // Check if br comes from the same module (not cloned)
    Module *brMod  = br->getParent()->getParent()->getParent();
    if (brMod==llvmMod) {
        BasicBlock *headerBB = getLoopLatch(br);
        if (headerBB) {
            return headerBB->getName().str();
        } else {
            return "";
        }
    }
    std::string brName  = br->getOperand(0)->getName().str();
    std::string funName = br->getParent()->getParent()->getName().str(); 
    std::map<BranchInst*,BasicBlock*>::iterator it;
    for (it=br2header.begin(); it!=br2header.end(); ++it) {
        BranchInst *b = it->first;
        if (b && b->getOperand(0)->getName()==brName 
            && b->getParent()->getParent()->getName()==funName) {
            BasicBlock *headerBB = it->second; 
            return headerBB->getName().str();
        }
    }
    return "";
}

// Return true if bb is a loop latch basicblock, else false.
// (A latch block is a block that contains a branch back to the header.)
bool LoopInfoPass::isLoopLatch(BasicBlock *bb) {
    std::map<BranchInst*,BasicBlock*>::const_iterator it;
    for (it=br2header.begin(); it!=br2header.end(); ++it) {
        const BasicBlock *latch = it->second;
        if (latch==bb) {
            return true;
        }
    }
    return false;
}

// Return all loop latch block branch instructions.
std::vector<BranchInst*> LoopInfoPass::getLoopLatchBrs() {
    std::vector<BranchInst*> brs;
    std::map<BranchInst*,BasicBlock*>::iterator it;
    for (it=br2header.begin(); it!=br2header.end(); ++it) {
       brs.push_back(it->first);
    }
    return brs;
}

bool LoopInfoPass::hasLoops() {
    return (br2header.size()>0);
}

bool LoopInfoPass::runOnFunction(Function &F) {
    
    // Retrieve information on loop for this function 
    LoopInfo *li = &getAnalysis<LoopInfo>();
    li->dump();
    // Return if there is no loops
    if(li->begin()==li->end())
        return false;
    MSG("=========== Loop Assert Pass ============\n");
    // For each loop in this function
    for (LoopInfo::iterator I = li->begin(), E = li->end(); I != E; ++I) {
        Loop *l = *I;
        // Tell if there are loops contained 
        // entirely within this loop
        if(l->getSubLoops().empty()) {
            MSG("  no subloops\n");
        } else {
            if(this->dbgMsg)
                std::cout << "  has " << l->getSubLoops().size() 
                << " subloops\n";
            const std::vector<Loop*> &subLoops = l->getSubLoops();
            for (size_t j = 0; j != subLoops.size(); j++) {
                assertLoop(subLoops[j]);
            }
        }
        assertLoop(l);
    }
    MSG("=========================================\n");
    return false;
}


/*bool LoopInfoPass::runOnFunction(Function &F) {
    
    bool changed = false;
    LoopInfo *li = &getAnalysis<LoopInfo>();
    
    if(!this->dbgMsg)
        return false;
    
    for (LoopInfo::iterator I = li->begin(), E = li->end(); I != E; ++I) {
        Loop *l = *I; 
        std::cout << "- Loop:\n";
        // Tell if there are loops contained entirely within this loop
        if(l->getSubLoops().empty()) {
            std::cout << "  no subloops\n";
        } else {
            std::cout << "  has " << l->getSubLoops().size() 
            << " subloops\n";
        }
        // Number of basic blocks which make up this loop
        if(!l->getBlocks().empty()) {
            std::cout << "  " << l->getBlocks().size() << " basic blocks\n";
        }
        // Number of back edges to the loop header
        std::cout << "  " << l->getNumBackEdges() << " back edges\n";
        // If getExitingBlocks would return exactly one block,
        // return that block. Otherwise return null
        if(l->getExitingBlock()) {
            std::string s = l->getExitingBlock()->getName();
            std::cout << "  " << s << " is the ExitingBlock\n";
        } else
            std::cout << "  there are more than one ExitingBlock\n";
    
        // Aloop has a preheader if there is only one edge to the header of 
        // the loop from outside of the loop.  If this is the case, 
        // the block branching to the header of the loop is the preheader node.
        if(!l->getLoopPreheader())
            std::cout << "  no preheader\n";
        else {
            std::string s = l->getLoopPreheader()->getName();
            std::cout << "  " << s << " is the preheader\n";
        }
        // If the given loop's header has exactly one unique
        // predecessor outside the loop, return it. Otherwise return null.
        // This is less strict that the loop "preheader" concept, which requires
        // the predecessor to have exactly one successor.
        if(!l->getLoopPredecessor())
            std::cout << "  no predecessor\n";
        else {
            std::string s = l->getLoopPredecessor()->getName();
            std::cout << "  " << s << " is the predecessor\n";
        }
        // If there is a single latch block for this loop, return it.
        // A latch block is a block that contains a branch back to the header.   
        if(!l->getLoopLatch())
            std::cout << "  no latch block\n";
        else {
            std::string s = l->getLoopLatch()->getName();
            std::cout << "  " << s << " is a latch block\n";
        }
        
        
    }
    return changed;
}*/