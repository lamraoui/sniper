/**
 * \file Frontend.cpp
 *
 * \author Si-Mohamed Lamraoui
 * \date   28 January 2015
 */

#include "Frontend.h"

const StringRef Frontend::SNIPER_FUN_PREFIX           = "sniper_";
const StringRef Frontend::SNIPER_ASSERT_FUN_NAME      = "sniper_assert";
const StringRef Frontend::SNIPER_ASSUME_FUN_NAME      = "sniper_assume";
const StringRef Frontend::SNIPER_LOOP_ASSERT_FUN_NAME = "sniper_assert_loop";


bool Frontend::run() {

    MSTimer timer;
    if (options->printDuration()) {
        timer.start();
    }
    
    // Set all functions with the AlwaysInline attribute
    // and inline all calls
    DataLayout  *DL = new DataLayout(llvmMod);
    PassManager *PM = new PassManager();
    PM->add(DL);
    PM->add(new FullFunctionInliningPass(options));
    PM->add(createFunctionInliningPass());
    PM->run(*llvmMod); // First round
    delete PM;
    
    // Save local variable names before processing the bytecode
    Function *targetFun = getFunction(options->getFunctionName());
    assert(targetFun && "Requested function missing");
    localVars = new LocalVariables();
    localVars->processLoadStore(targetFun);
    
    // Process global variables
    GlobalVariables *GV = new GlobalVariables();
    GV->process(targetFun);
    delete GV;
    
    // Unroll recursive calls
    /*DataLayout *DL1 = new DataLayout(llvmMod);
     PassManager *PM1 = new PassManager();
     PM1->add(DL1);
     PM1->add(createTailCallEliminationPass());
     PM1->run(*llvmMod); */
    
    // Promote alloca instructions to SSA registers
    // & Assign names to anonymous instructions
    FunctionPassManager *FPM = new FunctionPassManager(llvmMod);
    FPM->add(createPromoteMemoryToRegisterPass());
    FPM->add(createInstructionNamerPass());
    // Unroll loops
    FPM->add(createLoopSimplifyPass());
    FPM->add(createLoopRotatePass());
    FPM->add(createLCSSAPass());
    // Unroll loops regardless of how much code expansion would result
    int Threshold = UINT_MAX;
    int Count = options->getUnrollCount();
    int AllowPartial = 1; // true
    FPM->add(createLoopUnrollPass(Threshold, Count, AllowPartial));
    //FPM->add(createCFGSimplificationPass());
    FPM->doInitialization();
    FPM->run(*targetFun);
    delete FPM;
    
    // Get information about loops
    FunctionPassManager *FPM2 = new FunctionPassManager(llvmMod);
    loopInfo = new LoopInfoPass(llvmMod, options->dbgMsg());
    FPM2->add(loopInfo);
    FPM2->doInitialization();
    FPM2->run(*targetFun);
    //TODO: delete FPM2;
    
    // Process returns
    processFunctionReturns(targetFun);
    
    checkFunctionTypeSignature(targetFun);
    
    if (options->printModIR()) {
        std::cout << "---- Module IR -------------------------------\n";
        // Redirect the IR dump to stdout
        std::string irdump_str;
        llvm::raw_string_ostream rso(irdump_str);
        llvmMod->print(rso, NULL);
        std::cout << irdump_str << std::flush;
        std::cout << "----------------------------------------------\n\n";
    }
    else if (options->printFunIR()) {
        std::cout << "---- Target Function IR ----------------------\n";
        // Redirect the IR dump to stdout
        std::string irdump_str;
        llvm::raw_string_ostream rso(irdump_str);
        targetFun->print(rso);
        std::cout << irdump_str << std::flush;
        std::cout << "----------------------------------------------\n\n";
    }
    
    // Print CFG in dot
    if (options->outputCFGDotFile()) {
        FunctionPassManager *FPM3 = new FunctionPassManager(llvmMod);
        FPM3->add(createCFGPrinterPass());
        FPM3->doInitialization();
        FPM3->run(*targetFun);
        delete FPM3;
    }
    // Stop the timer
    if (options->printDuration()) {
        timer.stop("Frontend Execution Time");
    }
    return true;
}

Module *Frontend::getLLVMModule() {
    assert(llvmMod && "No LLVM module available");
    return this->llvmMod;
}

Function *Frontend::getFunction(std::string name) {
    assert(llvmMod && "No LLVM module available");
    return llvmMod->getFunction(name);
}

LocalVariables *Frontend::getLocalVars() {
    assert(localVars && "No local variables object");
    return localVars;
}

LoopInfoPass *Frontend::getLoopInfo() {
    assert(loopInfo && "No loop info object");
    return loopInfo;
}

void Frontend::checkFunctionTypeSignature(Function *f) {
    assert(f && "Expecting a function as argument!");
    // Check the number of args
    unsigned size = f->arg_size();
    assert(size>0 && "Expecting function to have arguments!");
    // Check function return type
    const Type *retTy = f->getReturnType();
    assert((!f->isVarArg() &&
            (retTy->isIntegerTy(32) || retTy->isVoidTy())) &&
           "Function return type not supported!");
    // Check the type of the arguments
    const FunctionType *FTy = f->getFunctionType();
    Function::arg_iterator ait;
    for (ait = f->arg_begin(); ait != f->arg_end(); ++ait) {
        const unsigned argNo = ait->getArgNo();
        const Type *argTy = FTy->getParamType(argNo);
        assert(argTy->isIntegerTy(32)
               && "Function argument types not supported!");
    }
}

void Frontend::processFunctionReturns(Function *f) {
    std::vector<Instruction*> worklist;
    for (inst_iterator I = inst_begin(f), E = inst_end(f); I != E; ++I) {
        /*MDNode *N = I->getMetadata("dbg");
         if (N) {
         DILocation Loc(N);
         unsigned line = Loc.getLineNumber();
         std::cout << line << ", ";
         }*/
        if(I->getOpcode()==Instruction::Add
           && I->getName().substr(0, 4) == "_ret") {
            worklist.push_back(&*I);
        }
    }
    //std::cout << std::endl;
    // Replace the dummy instructions _retx = v + v by
    // _retx = v + 0
    for (Instruction *I : worklist) {
        Value *V = I->getOperand(0);
        Value *zeroVal = ConstantInt::get(V->getType(), 0);
        BinaryOperator *addInst = BinaryOperator::CreateAdd(V, zeroVal);
        // Set debugging metadata
        MDNode *N = I->getMetadata("dbg");
        if (N) {
            addInst->setMetadata("dbg", N);
        } else {
            I->dump();
            std::cout << "warning : metadata!" << std::endl;
        }
        BasicBlock::iterator ii(I);
        ReplaceInstWithInst(I->getParent()->getInstList(), ii, addInst);
    }
}
