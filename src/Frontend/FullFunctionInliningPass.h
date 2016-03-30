/**
 * \file FullFunctionInliningPass.h
 *
 * ----------------------------------------------------------------------
 *                SNIPER : Automatic Fault Localization 
 *
 * Copyright (C) 2016 Si-Mohamed LAMRAOUI
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program (see LICENSE.TXT).  
 * If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
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
    /**
     * Default constructor.
     *
     * \param _options SNIPER options.
     */
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