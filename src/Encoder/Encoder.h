/**
 * Encoder.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/04/11
 * @copyright : NII 2014
 */

#ifndef _ENCODER_H
#define _ENCODER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set> 
#include <algorithm>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/DebugInfo.h"
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Support/InstIterator.h>
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/PostOrderIterator.h"

#include "Options.h"
#include "Utils/Utils.h"
#include "Logic/Expression.h"
#include "Logic/Formula.h"
#include "Frontend/LocalVariables.h"
#include "Frontend/LoopInfoPass.h"
#include "Profile/ProgramProfile.h"
#include "Encoder/Context.h"

using namespace llvm;


//============================================================================
class Encoder {

public:
    static const StringRef SNIPER_FUN_PREFIX;
    static const StringRef SNIPER_ASSERT_RETINT_FUN_NAME;
    static const StringRef SNIPER_ASSERT_RETVOID_FUN_NAME;
    static const StringRef SNIPER_ASSUME_RETINT_FUN_NAME;
    static const StringRef SNIPER_ASSUME_RETVOID_FUN_NAME;
    static const StringRef ASSERT_FUN_NAME;
    static const StringRef ASSUME_FUN_NAME;
    static const StringRef LOOP_ASSERT_FUN_NAME;
    
private:
    Context *ctx;
    
    
public:
    Encoder(Context *_ctx) : ctx(_ctx)  { }
    ~Encoder() { }
    
    ExprPtr encodeControlFlow(Function *targetFun);
    ExprPtr encode(BinaryOperator *bo);
    ExprPtr encode(SelectInst *select);
    ExprPtr encode(PHINode *phi, BasicBlock *forBlock = NULL);
    ExprPtr encode(BranchInst *br, LoopInfoPass *loops);
    ExprPtr encode(SwitchInst *s);
    ExprPtr encode(ICmpInst *icmp);
    ExprPtr encode(AllocaInst *alloca);
    ExprPtr encode(StoreInst *store);
    ExprPtr encode(LoadInst *load, Formula *formula);
    ExprPtr encode(GetElementPtrInst *gep);
    ExprPtr encode(SExtInst *sext);
    ExprPtr encode(ZExtInst *zext);
    ExprPtr encode(CallInst *call, Formula *formula);
    
    // Not implemented
    ExprPtr encode(FenceInst *i);
    ExprPtr encode(AtomicCmpXchgInst *i);    
    ExprPtr encode(AtomicRMWInst *i);    
    ExprPtr encode(FCmpInst *i);    
    ExprPtr encode(VAArgInst *i);
    ExprPtr encode(ExtractElementInst *i);
    ExprPtr encode(InsertElementInst *i); 
    ExprPtr encode(ShuffleVectorInst *i);    
    ExprPtr encode(ExtractValueInst *i);    
    ExprPtr encode(InsertValueInst *i);    
    ExprPtr encode(LandingPadInst *i);    
    ExprPtr encode(ReturnInst *i);   
    ExprPtr encode(IndirectBrInst *i);
    ExprPtr encode(InvokeInst *i);  
    ExprPtr encode(ResumeInst *i);  
    ExprPtr encode(UnreachableInst *i); 
    ExprPtr encode(TruncInst *i);  
    ExprPtr encode(FPTruncInst *i); 
    ExprPtr encode(FPExtInst *i);   
    ExprPtr encode(UIToFPInst *i); 
    ExprPtr encode(SIToFPInst *i); 
    ExprPtr encode(FPToUIInst *i);  
    ExprPtr encode(FPToSIInst *i);  
    ExprPtr encode(IntToPtrInst *i); 
    ExprPtr encode(PtrToIntInst *i); 
    ExprPtr encode(BitCastInst *i);  
    
private:
    bool isArgv(const Value* V);
    
};
//============================================================================


#endif