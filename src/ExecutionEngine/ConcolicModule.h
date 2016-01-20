/**
 * ConcolicModule.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2013/08/21
 * @copyright : NII 2013
 */


#ifndef _CONCOLICMODULE_H
#define _CONCOLICMODULE_H

#include <iostream>
#include <vector>
#include <queue>  
#include <list> 

#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/PassManager.h>
#include "llvm/ExecutionEngine/ObjectCache.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/CodeGen/MachineCodeInfo.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Value.h"

#include "Options.h"
#include "Utils/Utils.h"
#include "Frontend/FullFunctionInliningPass.h"
#include "Profile/ProgramProfile.h"
#include "Logic/Expression.h"
#include "Logic/Formula.h"
#include "Logic/YicesSolver.h"
#include "ExecutionEngine/IRInstrumentor.h"
#include "ExecutionEngine/Symbol.h"
#include "ExecutionEngine/SymbolMap.h"

using namespace llvm;

// Search strategy for selecting the node to expand
// - DFS: Depth-First Search
// - BFS: Breadth-First Search
// - GS : Generational Search
enum SearchStrategy {
    DFS = 0,
    BFS,
    GS
};

//============================================================================
class ConcolicModule {
        
protected:
    Module      *llvmMod;
    Function    *targetFun;
    Options     *options;
    IRBuilder<> *IRB; 
    YicesSolver *solver;
    unsigned    roundID;
    bool        terminated;
    VariablesPtr lastInputs;
    std::vector<std::pair<VariablesPtr,std::vector<State_t> > > pendingInputs;
    FormulaPtr  preConditions;
    
public:
    ConcolicModule(Module *_llvmMod, Function *_targetFun, Options *_options);
    virtual ~ConcolicModule();
    virtual void run(ProgramProfile *profile, LocalVariables *locVars, 
                     LoopInfoPass *loopInfo) = 0;

protected:
    ExecutionEngine* initialize();
    Value* callFunction(ExecutionEngine *EE, Module *m, Function *f,
                      std::vector<Value*> args);
    
};
//============================================================================

#endif
