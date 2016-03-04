/**
 * \file IRInstrumentor.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   24 February 2016
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

/**
 * \class IRInstrumentor
 *
 * This class is used to instrument an LLVM code 
 * so that the concolic execution can collect concrete 
 * and symbolic values at execution time. 
 */
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
    /**
     * Default constructor.
     *
     * \param _llvmMod An LLVM module in which the target function is.
     * \param _EE An LLVM execution engine to run (JIT) the target function.
     */
    IRInstrumentor(Module *_llvmMod, ExecutionEngine *_EE);
    /**
     * Destructor.
     */
    ~IRInstrumentor() { }
    
    /**
     * Instrument each functions in the LLVM module \p llvmMod 
     * except the llvm functions (intrinsic) or the functions 
     * with an unknown body.
     *
     * \param llvmMod A LLVM module to instrument.
     * \param targetFun An LLVM function.
     */
    void instrumentModule(Module *llvmMod, Function *targetFun);
    
private:
    /**
     * Instrument the function \p f as a preliminary 
     * step to do concolic execution or execution profilling. 
     *
     * \param f The LLVM function to be instrumented.
     * \param isTarget Is true when the \p f is the target function, 
     * false otherwise.
     */
    void instrumentFunction(Function *f, bool isTarget);
    
    /**
     * Insert a profiling function before 
     * the LLVM alloca instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecAlloca(Instruction *i);
    /**
     * Insert a profiling function before 
     * the LLVM GEP instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecGep(Instruction *i);
     /**
     * Insert a profiling function before 
     * the LLVM load instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecLoad(Instruction *i);
     /**
     * Insert a profiling function before 
     * the LLVM store instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecStore(Instruction *i);
     /**
     * Insert a profiling function before 
     * the LLVM cast instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecCastOp(Instruction *i);
     /**
     * Insert a profiling function before 
     * the LLVM call instruction \p i.
     *
     * \param i An instruction to instrument. 
     * \param isTarget True if the function 
     * being called is the target function.
     */
    bool insertExecCall(Instruction *i, bool isTarget);
    /**
     * Insert a profiling function before 
     * the LLVM binary instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecBinOp(Instruction *i);
    /**
     * Insert a profiling function before 
     * the LLVM phi instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecPhi(Instruction *i);
    /**
     * Insert a profiling function before 
     * the LLVM select instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecSelect(Instruction *i);
    /**
     * Insert a profiling function before 
     * the LLVM br instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertExecBr(Instruction *i);
    /**
     * Insert a profiling function before 
     * the LLVM return instruction \p i.
     *
     * \param i An instruction to instrument. 
     */
    void insertReportEnd(Instruction *i);
    /**
     * Replace in \p targetFun all call to sniper_assume,  
     * sniper_assert and exit functions by profiling functions.
     */    
    void replaceAssertAssumeExitCall(Function *targetFun);

    void callFunction(Function *f, std::vector<Value*> args);

};

#endif // _IRINSTRUMENTOR_H
