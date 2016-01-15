/**
 * IRInstrumentor.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
 */


#ifndef _IRINSTRUMENTOR_H
#define _IRINSTRUMENTOR_H

#include <iostream>
#include <stdbool.h>

#include "llvm/IR/Instructions.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/CodeGen/MachineCodeInfo.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "Frontend/Frontend.h"
#include "Executor.h"

using namespace llvm;

//============================================================================
class IRInstrumentor {
        
private:
    Module *llvmMod;
    ExecutionEngine *EE;
    Function *ExecInstFun;
    Function *ExecInst2i64Fun;
    Function *ExecInst1i32Fun;
    Function *ExecInst2i32Fun;
    Function *ExecInst3i32Fun;
    Function *ExecInst1i1Fun;
    Function *ExecInst2i1Fun;
    Function *ExecInst3i1Fun;
    Function *ExecInst1i1and2i32Fun;
    Function *ReportEndFun;
    Function *ReportAssertFun;
    Function *ReportAssumeFun;
    Function *PushArgsFun;
    Function *PopArgsFun;

public:
    IRInstrumentor(Module *_llvmMod, ExecutionEngine *_EE);
    void instrumentModule(Module *llvmMod, Function *targetFun);
    ~IRInstrumentor() { }
    
private:
    void instrumentFunction(Function *f, bool isTarget);
    
    void insertExecAlloca(Instruction *i);
    void insertExecGep(Instruction *i);
    void insertExecLoad(Instruction *i);
    void insertExecStore(Instruction *i);
    void insertExecCastOp(Instruction *i);
    bool insertExecCall(Instruction *i, bool isTarget);
    void insertExecBinOp(Instruction *i);
    void insertExecPhi(Instruction *i);
    void insertExecSelect(Instruction *i);
    void insertExecBr(Instruction *i);
    void insertReportEnd(Instruction *i);
    
    void callFunction(Function *f, std::vector<Value*> args);
    void replaceAssertAssumeExitCall(Function *targetFun);
};
//============================================================================

#endif
