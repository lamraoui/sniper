/**
 * \file ConcolicModule.h
 *
 * \author Si-Mohamed Lamraoui
 * \date   22 February 2016
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

/**
 * Search strategy for selecting 
 * the node to expand.
 */
enum SearchStrategy {
    DFS = 0, /*< Depth-First Search */
    BFS,     /*< Breadth-First Search */
    GS       /*< Generational Search */
};

/**
 * \class ConcolicModule
 *
 * This class provides a base class to 
 * implement a concolic execution algorithm. 
 *
 * Concolic execution can be used to generate 
 * program executions with a high code coverage. 
 * Concolic execution was proposed as a dynamic 
 * testing method that simultaneously performs 
 * symbolic and concrete execution. This method 
 * provides a high code coverage for program testing. 
 * We follow the approach of CUTE. The algorithm 
 * first executes the program on some random input values. 
 * The method keeps track of a path constraint (PC) 
 * to collect symbolic predicate expressions from 
 * branching points. The conjunction of the predicates 
 * in PC holds for the execution path. The CUTE 
 * algorithm checks whether the PC with the last 
 * constraint negated is satisfiable. If so, new input 
 * values are generated allowing each test run to 
 * exercise different program paths. The algorithm 
 * stops when either no more new inputs can be generated 
 * or a given time-out is reached.
 * This module benefits from the JIT capabilities of LLVM 
 * to run the program in an efficient manner. 
 * It implies that CONCOLIC test generation can be efficient 
 * by consuming less time to terminate.
 */
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
    
public:
    ConcolicModule(Module *_llvmMod, Function *_targetFun, Options *_options) 
    : llvmMod(_llvmMod), targetFun(_targetFun), options(_options), lastInputs(NULL) {
        // Create an instruction builder
        LLVMContext &Context = getGlobalContext();
        this->IRB =  new IRBuilder<>(Context);
        // Set the maximum depth for the executions
        Executor::setMaxDepth(options->getMaxDepth());
    }
    virtual ~ConcolicModule() {
        delete IRB;
        delete this->solver;
    }

    /**
     * Run concolic execution algorithm on the function targetFun.
     *
     * \param profile A program profile to store program information.
     * \param locVars Information on local variables in targetFun.
     * \param loopInfo Information on loops in targetFun.
     */
    virtual void run(ProgramProfile *profile, LocalVariables *locVars, 
                     LoopInfoPass *loopInfo) = 0;

protected:
    /**
     * Initialize the concolic execution algorithm.
     *
     * \return An LLVM execution engine to run the target function.
     */
    ExecutionEngine* initialize();
    /**
     * Execute the function \p f using the values \p args as 
     * arguments. 
     *
     * \param EE A LLVM execution engine.
     * \param m The parent LLVM module of \p f.
     * \param args The values to pass as arguments. 
     * \return The concrete value returned by the execution of \p f.
     */
    Value* callFunction(ExecutionEngine *EE, Module *m, Function *f,
                      std::vector<Value*> args);
    
};

#endif // _CONCOLICMODULE_H
