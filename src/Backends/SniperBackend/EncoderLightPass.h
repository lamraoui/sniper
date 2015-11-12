/**
 * EncoderLightPass.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/04/04
 * @copyright : NII 2013
 */

#ifndef _ENCODERLIGHTPASS_H
#define _ENCODERLIGHTPASS_H

#include <map>
#include <ctime>
#include <sys/time.h>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

#include "Options.h"
#include "Frontend/LoopInfoPass.h"
#include "Profile/ProgramProfile.h"
#include "Encoder/Context.h"
#include "Encoder/EncoderLight.h"

using namespace llvm;


//============================================================================
class EncoderLightPass {
    
private:
    Function *targetFun;
    Context *ctx;
    EncoderLight *encoder;
    LoopInfoPass *loops;
    ProgramProfile * profile;
    Options *options;
    Formula *AS;

public:
    EncoderLightPass(Function *_targetFun, Context *_ctx, LoopInfoPass *_loops,
                ProgramProfile *profile, Options *options);
    ~EncoderLightPass();
    Formula* makeTraceFormula();
    Formula* getASFormula();
    
private:
    void initGlobalVariables();
    void initAssertCalls();
    
};
//============================================================================


//============================================================================
// Static path from bb1 to bb2
// The block pred is the block executed before bb1
// The block next is the block executed after bb2;
class Path {
    
public:
    BasicBlock *pred;
    BasicBlock *bb1;
    std::vector<ExprPtr> clauses;
    BasicBlock *bb2;
    BasicBlock *next;

public:
    
    bool equal(Path *p) {
        if (p==NULL)       return false;
        if (pred!=p->pred) return false;
        if (bb1!=p->bb1)   return false;
        if (bb2!=p->bb2)   return false;
        if (next!=p->next) return false;
        if (clauses.size()!=p->clauses.size()) return false;
        for (unsigned i=0; i<clauses.size(); i++) {
            if (clauses[i] != p->clauses[i]) {
                return false;
            }
        }
        return true;
    }
    // Return true iff p has the same entry transition
    bool hasSameEntryTrans(Path *p) {
        if (p==NULL) return false;
        return (pred==p->pred && bb1==p->bb1);
    }
    // Return true iff p has the same exit transition
    bool hasSameExitTrans(Path *p) {
        if (p==NULL) return false;
        return (bb2==p->bb2 && next==p->next);
    }
    void dump() {
        if (pred) {
            std::cout << pred->getName().str() << "_";
        } else {
            std::cout << "null_";
        }
        if (bb1) {
            std::cout << bb1->getName().str() << " : ";
        } else {
            std::cout << "null : ";
        }
        for (ExprPtr e : clauses) {
            e->dump();
            std::cout << " ";
        }
        if (bb2) {
            std::cout << " : " << bb2->getName().str() << "_";
        } else {
            std::cout << " : null_";
        }
        if (next) {
            std::cout << next->getName().str() << std::endl;
        } else {
            std::cout << "null\n";
        }
    }
    
};
//============================================================================

#endif