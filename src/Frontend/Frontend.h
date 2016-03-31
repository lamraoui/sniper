/**
 * \file Frontend.h
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

#ifndef _FRONTEND_H
#define _FRONTEND_H

#include <string>

#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/DebugInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/CFGPrinter.h>
#include <llvm/PassManager.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Support/TargetSelect.h>

#include "Options.h"
#include "Frontend/LocalVariables.h"
#include "Frontend/FullFunctionInliningPass.h"
#include "Frontend/GlobalVariables.h"
#include "Frontend/LoopInfoPass.h"

using namespace llvm;

/**
 * \class Frontend
 *
 * \brief This class provides functions that pre-process the input program.
 *
 * The pre-processing consist of inlining function calls, puting the intermediate
 * representation in SSA form, and unwinding loops fully (if prossible) or
 * to given a bound.
 */
class Frontend {

public:
    /**
     * Prefix of special SNIPER's function names.
     * <a>sniper_XXXX()</a>
     */
    static const StringRef SNIPER_FUN_PREFIX;
    /**
     * Name of the special function that defines a post-condition.
     * <a>sniper_assert(bool)</a>
     */
    static const StringRef SNIPER_ASSERT_FUN_NAME;
    /**
     * Name of the special function that defines a pre-condition.
     * <a>sniper_assume(bool)</a>
     */
    static const StringRef SNIPER_ASSUME_FUN_NAME;
    /**
     * Name of the special function that defines a loop invariant.
     * <a>sniper_loop(bool)</a>
     */
    static const StringRef SNIPER_LOOP_ASSERT_FUN_NAME;
    
private:
    /**
     * The LLVM module where is the target function.
     */
    Module *llvmMod;
    /**
     * User defined options.
     */
    Options *options;
    /**
     * Information about the local variables in the target function.
     */
    LocalVariables *localVars;
    /**
     * Information about the loops in the target function.
     */
    LoopInfoPass  *loopInfo;

public:
    /**
     * Default constructor.
     *
     * \param _llvmMod An LLVM module in which the target function is.
     * \param _options SNIPER options.
     */
    Frontend(Module *llvmMod, Options *options) :
    llvmMod(llvmMod), options(options) {}
    /**
     * Destructor.
     */
     ~Frontend() { }
    
    /**
     * Pre-process the target function.
     *
     * This function inlines all function calls, transforms the intermediate
     * representation in SSA form, and unwinds all loops fully (if prossible) or
     * to given a bound (see options).
     */
    bool run();
    /**
     * Return the LLVM module where is the target function.
     */
    Module *getLLVMModule();
    /**
     * Return the LLVM target function.
     */
    Function *getFunction(std::string name);
    /**
     * Return information about local variables.
     */
    LocalVariables *getLocalVars();
    /**
     * Return information about loops.
     */
    LoopInfoPass *getLoopInfo();
    
private:
    /**
     * Check whether or not the target function's type signature is 
     * supported. In case the signature is not supported, 
     * the program exits.
     *
     * SNIPER supports the following type signatures:
     * <a> [void|int32] foo([int32...int32]) </a>
     */
    void checkFunctionTypeSignature(Function *f);
    /**
     * Add a dummy add instruction before each function return.
     */
    void processFunctionReturns(Function *f);

};
    
#endif // _FRONTEND_H