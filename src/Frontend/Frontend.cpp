/**
 * Frontend.cpp
 *
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2015/07/08
 * @copyright : NII 2015
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
    localVars = new LocalVariables(targetFun);
    localVars->processLoadStore();
    
    // Process global variables
    GlobalVariables *GV = new GlobalVariables(targetFun, options->dbgMsg());
    GV->process();
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
    
    // Process the phi instructions
    localVars->processPhi();
    // Process returns
    processFunctionReturns(targetFun);
    
    // Check the signature of the target function
    // and deppending on the type try to simplify the function
    argvUsed = isStandardFunctionSignature(targetFun);
    if (argvUsed) {
        //bool simplified = simplifyFunctionSignature(targetFun);
        //if (simplified) {
        //    std::cout << "info: the function was successfully simplified!\n";
        //}
    } else {
        checkFunctionSignature(targetFun);
    }
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

bool Frontend::hasArgv() {
    return argvUsed;
}


// ============================================================================
// simplifyFunctionSignature
//
// Simplify the main function signature from (int argc, char **argv)
// to its equivalente form of type (int arg1, int arg2, ..., argn).
//
// return true if the target function is of type main(argc,argv) and
//              if we are able to simplify the function,
// else return false.
// ============================================================================
bool Frontend::simplifyFunctionSignature(Function *targetFun) {
    
    // Get the argv function argument
    Function::arg_iterator ait = targetFun->arg_begin();
    Value *argv = ++ait;
    
    // Start by localizing the argv pointer accesses
    // %ptr = getelementptr i8** %argv, i64 i
    std::vector<Value*> argvUse;
    for (Value::use_iterator i1 = argv->use_begin(), e1 = argv->use_end(); i1 != e1; ++i1) {
        Value *V = *i1;
        GetElementPtrInst *G = dyn_cast<GetElementPtrInst>(V);
        if (!G) {
            return false;
        }
        for (Value::use_iterator i2 = G->use_begin(), e2 = G->use_end(); i2 != e2; ++i2) {
            Value *V2 = *i2;
            LoadInst *L = dyn_cast<LoadInst>(V2);
            if (!L) {
                return false;
            }
            for (Value::use_iterator i3 = L->use_begin(), e3 = L->use_end(); i3 != e3; ++i3) {
                Value *V3 = *i3;
                CallInst *C = dyn_cast<CallInst>(V3);
                if (!C) {
                    return false;
                }
                Function *F = C->getCalledFunction();
                if (!F || F->getName()!="atoi") {
                    return false;
                }
                C->dump();
            }
            
        }
    }
    
    // Create a function type
    int nbArg = 6;
    Type *i32Ty = IntegerType::get(llvmMod->getContext(), 32);
    std::vector<Type*> funcTy;
    for (int i=0; i<nbArg; i++) {
        funcTy.push_back(i32Ty);
    }
    FunctionType* FT =
    FunctionType::get(i32Ty, funcTy, false);
    Constant *C = llvmMod->getOrInsertFunction("targetFunction", FT);
    Function *newTargetFun = cast<Function>(C);
    newTargetFun->setCallingConv(CallingConv::C);
    
    
    
    std::vector<Type*> ArgTypes;
    ValueToValueMapTy VMap;
    //FunctionType *FTy = FunctionType::get(F.getFunctionType()->getReturnType(), ArgTypes, F.getFunctionType()->isVarArg());
    //Function *NewF = Function::Create(FTy, F.getLinkage(), F.getName());
    SmallVector<ReturnInst*, 8> Returns;
    CloneFunctionInto(newTargetFun, targetFun, VMap, false, Returns, "_", 0, 0);
    
    newTargetFun->dump();
    
    /*
     std::vector<Value*> argvUse2;
     std::vector<Value*>::const_iterator it;
     for (it=argvUse.begin(); it!=argvUse.end(); ++it) {
     Value *V = *it;
     for (Value::use_iterator i = V->use_begin(), e = V->use_end(); i != e; ++i) {
     Value *V2 = *i;
     if (LoadInst *L = dyn_cast<LoadInst>(V2)) {
     argvUse2.push_back(V2);
     } else {
     std::cout << "error: wrong usage of argv!\n";
     exit(1);
     }
     }
     }
     std::vector<Value*> argvUse3;
     std::vector<Value*>::const_iterator it2;
     for (it2=argvUse2.begin(); it2!=argvUse2.end(); ++it2) {
     Value *V = *it2;
     for (Value::use_iterator i = V->use_begin(), e = V->use_end(); i != e; ++i) {
     Value *V2 = *i;
     if (CallInst *C = dyn_cast<CallInst>(V2)) {
     argvUse3.push_back(V2);
     } else {
     std::cout << "error: wrong usage of argv!\n";
     exit(1);
     }
     }
     }*/
    
    
    return true;
}

// ============================================================================
// isStandardFunctionSignature
//
// return true if the function is of type: int main(int argc, char **argv)
// else return false.
// =============================================================================
bool Frontend::isStandardFunctionSignature(Function *targetFun) {
    if (!targetFun->getReturnType()->isIntegerTy(32)) {
        return false;
    }
    if (targetFun->arg_size()!=2) {
        return false;
    }
    Function::arg_iterator ait = targetFun->arg_begin();
    Value *argc = ait++;
    if (!argc->getType()->isIntegerTy(32)) {
        return false;
    }
    Value *argv = ait;
    const Type *T = argv->getType();
    if(!T->isPointerTy() || T->getNumContainedTypes()!=1) {
        return false;
    }
    const Type *T2 = T->getContainedType(0);
    if (!T2->isPointerTy() || T2->getNumContainedTypes()!=1) {
        return false;
    }
    const Type *T3 = T2->getContainedType(0);
    if (!T3->isIntegerTy(8)) {
        return false;
    }
    return true;
}


// ============================================================================
// checkFunctionSignature
//
// Check if the function signature can be handle by SNIPER's formula encoder.
// Exit the program if the function signature is not supported.
// =============================================================================
void Frontend::checkFunctionSignature(Function *targetFun) {
    assert(targetFun && "Expecting a function as argument!");
    // Check the number of args
    unsigned size = targetFun->arg_size();
    assert(size>0 && "Expecting function to have arguments!");
    // Check function return type
    const Type *retTy =  targetFun->getReturnType();
    assert((!targetFun->isVarArg() &&
            (retTy->isIntegerTy(32) || retTy->isVoidTy())) &&
           "Function return type not supported!");
    // Check the type of the arguments
    const FunctionType *FTy = targetFun->getFunctionType();
    Function::arg_iterator ait;
    for (ait = targetFun->arg_begin(); ait != targetFun->arg_end(); ++ait) {
        const unsigned argNo = ait->getArgNo();
        const Type *argTy = FTy->getParamType(argNo);
        assert(argTy->isIntegerTy(32) && "Function argument types not supported!");
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
