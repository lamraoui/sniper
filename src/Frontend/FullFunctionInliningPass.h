/**
 * \file FullFunctionInliningPass.h
 * 
 * \author Si-Mohamed Lamraoui
 * \date   28 January 2015
 */

#ifndef FullFunctionInliningPass_H
#define FullFunctionInliningPass_H

#include <iostream>
#include <string>

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/DebugInfo.h"
#include "llvm/PassManagers.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Analysis/Verifier.h"

#include "Options.h"

using namespace llvm;

/**
 * \class FullFunctionInliningPass
 *
 * This class is an LLVM pass that inline function calls.
 * Inlining a function call consist of replacing a call instruction by 
 * the callee function bodies.
 */
class FullFunctionInliningPass : public FunctionPass {
    
    /**
     * LLVM pass ID.
     */
    static char ID;
    
private:
    /**
     * User defined options.
     */
    Options *options;
    
public:
    FullFunctionInliningPass(Options *_options) 
    : FunctionPass(ID), options(_options) { }
    /**
     * Destructor.
     */
    ~FullFunctionInliningPass() { }
    
    /**
     * Return an LLVM pass that can be used to inline function calls.
     *
     * \param o User's options.
     */
    Pass *createFullFunctionInliningPass(Options *o);
    /**
     * Inline all function calls in \p F.
     *
     * \param F An LLVM function.
     */
    virtual bool runOnFunction(Function &F);
    
};

#endif // FullFunctionInliningPass_H