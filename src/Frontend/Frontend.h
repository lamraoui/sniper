/**
 * Frontend.h
 *
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/07/08
 * @copyright : NII 2015
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

//============================================================================
class Frontend {
    
private:
    Module          *llvmMod;
    Options         *options;
    LocalVariables  *localVars;
    LoopInfoPass    *loopInfo;
    bool argvUsed;

public:
    Frontend(Module *llvmMod, Options *options) :
    llvmMod(llvmMod), options(options) {}
    bool            run();
    Module          *getLLVMModule();
    Function        *getFunction(std::string name);
    LocalVariables  *getLocalVars();
    LoopInfoPass    *getLoopInfo();
    bool            hasArgv();
    
private:
    bool isStandardFunctionSignature(Function *targetFun);
    bool simplifyFunctionSignature(Function *targetFun);
    void checkFunctionSignature(Function *targetFun);
    void processFunctionReturns(Function *f);

};
//============================================================================
    
#endif