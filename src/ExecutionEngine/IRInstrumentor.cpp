/**
 * \file IRInstrumentor.cpp
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
 * \date   30 March 2016
 */

#include "IRInstrumentor.h"

// Instruction with no argument 
void sniper_executeInst(intptr_t inst) {
    Instruction *i = (Instruction*) inst;
    Executor::ExecuteInst(i);
}
// Instruction with one argument of type int32
void sniper_executeInst1_i32(intptr_t inst, int arg) {
    Instruction *i = (Instruction*) inst;
    LLVMContext &Context = getGlobalContext();
    IRBuilder<> IRB(Context);
    Value *val = IRB.getInt32(arg);
    Executor::ExecuteInst1(i, val);
}
// Instruction with two arguments of type int32
void sniper_executeInst2_i32(intptr_t inst, int arg1, int arg2) {
    Instruction *i = (Instruction*) inst;
    LLVMContext &Context = getGlobalContext();
    IRBuilder<> IRB(Context);
    Value *val1 = IRB.getInt32(arg1);
    Value *val2 = IRB.getInt32(arg2);
    Executor::ExecuteInst2(i, val1, val2);
}
// Instruction with two argument of type int64
void sniper_executeInst2_i64(intptr_t inst, int arg1, int arg2) {
    Instruction *i = (Instruction*) inst;
    LLVMContext &Context = getGlobalContext();
    IRBuilder<> IRB(Context);
    Value *val1 = IRB.getInt64(arg1);
    Value *val2 = IRB.getInt64(arg2);
    Executor::ExecuteInst2(i, val1, val2);
}
// Instruction with three argument of type int32
void sniper_executeInst3_i32(intptr_t inst, int arg1, int arg2, int arg3) {
    Instruction *i = (Instruction*) inst;
    LLVMContext &Context = getGlobalContext();
    IRBuilder<> IRB(Context);
    Value *val1 = IRB.getInt32(arg1);
    Value *val2 = IRB.getInt32(arg2);
    Value *val3 = IRB.getInt32(arg3);
    Executor::ExecuteInst3(i, val1, val2, val3);
}
// Instruction with one argument of type boolean
void sniper_executeInst1_i1(intptr_t inst, bool arg) {
    Instruction *i = (Instruction*) inst;
    LLVMContext &Context = getGlobalContext();
    IRBuilder<> IRB(Context);
    Value *val = IRB.getInt1(arg);
    Executor::ExecuteInst1(i, val);
}
// Instruction with two argument of type boolean
void sniper_executeInst2_i1(intptr_t inst, bool arg1, bool arg2) {
    Instruction *i = (Instruction*) inst;
    LLVMContext &Context = getGlobalContext();
    IRBuilder<> IRB(Context);
    Value *val1 = IRB.getInt1(arg1);
    Value *val2 = IRB.getInt1(arg2);
    Executor::ExecuteInst2(i, val1, val2);
}
// Instruction with three argument of type boolean
void sniper_executeInst3_i1(intptr_t inst, bool arg1, bool arg2, bool arg3) {
    Instruction *i = (Instruction*) inst;
    LLVMContext &Context = getGlobalContext();
    IRBuilder<> IRB(Context);
    Value *val1 = IRB.getInt1(arg1);
    Value *val2 = IRB.getInt1(arg2);
    Value *val3 = IRB.getInt1(arg3);
    Executor::ExecuteInst3(i, val1, val2, val3);
}
// Instruction with one boolean arg and two int32 args
void sniper_executeInst1_i1_2_i32(intptr_t inst, bool arg1, int arg2, int arg3) {
    Instruction *i = (Instruction*) inst;
    LLVMContext &Context = getGlobalContext();
    IRBuilder<> IRB(Context);
    Value *val1 = IRB.getInt1(arg1);
    Value *val2 = IRB.getInt32(arg2);
    Value *val3 = IRB.getInt32(arg3);
    Executor::ExecuteInst3(i, val1, val2, val3);
}

void sniper_reportEnd() {
    Executor::ReportEnd();
}

void sniper_reportAssert(intptr_t *v, bool r) {
    Value *val = (Value*) v;
    Executor::ReportAssert(val, r);
}

void sniper_reportAssume(intptr_t *v, bool r) {
    Value *val = (Value*) v;
    Executor::ReportAssume(val, r);
}

void sniper_pushArgs(intptr_t arg) {
    Value *v = (Value*) arg;
    Executor::PushArgs(v);
}

void sniper_popArgs(intptr_t arg) {
    Value *v = (Value*) arg;
    Executor::PopArgs(v);
}


IRInstrumentor::IRInstrumentor(Module *_llvmMod, ExecutionEngine *_EE) 
: llvmMod(_llvmMod), EE(_EE) {

    LLVMContext &Context = llvmMod->getContext();   
    Type *voidTy = Type::getVoidTy(Context);
    Type *int1Ty = Type::getInt1Ty(Context);
    Type *int32Ty = Type::getInt32Ty(Context);
    Type *int64Ty = Type::getInt64Ty(Context);
    
    // Register the ExecuteInst functions
    ExecInstFun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst",
                                                voidTy, int64Ty, NULL));
    EE->addGlobalMapping(ExecInstFun, (void *)sniper_executeInst);
    // Register the ExecuteInst1_i32 functions
    ExecInst1i32Fun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst1_i32", 
                                                voidTy, int64Ty, int32Ty, NULL));
    EE->addGlobalMapping(ExecInst1i32Fun, (void *)sniper_executeInst1_i32);
    // Register the ExecuteInst2_i32 functions
    ExecInst2i32Fun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst2_i32", 
                                                voidTy, int64Ty, int32Ty,
                                                int32Ty, NULL));
    EE->addGlobalMapping(ExecInst2i32Fun, (void *)sniper_executeInst2_i32);  
    // Register the ExecuteInst2_i64 functions
    ExecInst2i64Fun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst2_i64", 
                                                voidTy, int64Ty, int64Ty,
                                                int64Ty, NULL));
    EE->addGlobalMapping(ExecInst2i64Fun, (void *)sniper_executeInst2_i64);  
    // Register the ExecuteInst3_i32 functions                        
    ExecInst3i32Fun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst3_i32", 
                                                voidTy, int64Ty, int32Ty,
                                                int32Ty, int32Ty, NULL));
    EE->addGlobalMapping(ExecInst3i32Fun, (void *)sniper_executeInst3_i32);  
    // Register the ExecuteInst1_i1 functions
    ExecInst1i1Fun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst1_i1", 
                                                voidTy, int64Ty, int1Ty, NULL));
    EE->addGlobalMapping(ExecInst1i1Fun, (void *)sniper_executeInst1_i1);
    // Register the ExecuteInst2_i1 functions
    ExecInst2i1Fun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst2_i1", 
                                                voidTy, int64Ty, int1Ty,
                                                int1Ty, NULL));
    EE->addGlobalMapping(ExecInst2i1Fun, (void *)sniper_executeInst2_i1);  
    // Register the ExecuteInst3_i1 functions                        
    ExecInst3i1Fun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst3_i1", 
                                                voidTy, int64Ty, int1Ty,
                                                int1Ty, int1Ty, NULL));
    EE->addGlobalMapping(ExecInst3i1Fun, (void *)sniper_executeInst3_i1);  
    // Register the ExecuteInst1_i1_2_i32 functions                        
    ExecInst1i1and2i32Fun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_executeInst1_i1_2_i32", 
                                                voidTy, int64Ty, int1Ty,
                                                int32Ty, int32Ty, NULL));
    EE->addGlobalMapping(ExecInst1i1and2i32Fun,
                         (void *)sniper_executeInst1_i1_2_i32);
    // Register the ReportEnd function
    ReportEndFun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_reportEnd", voidTy, NULL));
    EE->addGlobalMapping(ReportEndFun, (void *)sniper_reportEnd); 
    // Register the sniper_reportAssert function
    ReportAssertFun =
    cast<Function>(llvmMod->getOrInsertFunction("sniper_reportAssert",
                                                voidTy, int64Ty, int1Ty, NULL));
    EE->addGlobalMapping(ReportAssertFun, (void *)sniper_reportAssert);
    // Register the sniper_reportAssume function
    ReportAssumeFun =
    cast<Function>(llvmMod->getOrInsertFunction("sniper_reportAssume",
                                                voidTy, int64Ty, int1Ty, NULL));
    EE->addGlobalMapping(ReportAssumeFun, (void *)sniper_reportAssume);
    // Register the sniper_pushArgs function
    PushArgsFun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_pushArgs", voidTy,
                                                int64Ty, NULL));
    EE->addGlobalMapping(PushArgsFun, (void *)sniper_pushArgs); 
    // Register the sniper_popArgs function
    PopArgsFun = 
    cast<Function>(llvmMod->getOrInsertFunction("sniper_popArgs", voidTy,
                                                int64Ty, NULL));
    EE->addGlobalMapping(PopArgsFun, (void *)sniper_popArgs); 
}


//
// Instrument each function of the the given module
//
void IRInstrumentor::instrumentModule(Module *llvmMod, Function *targetFun) {

    Module::iterator it;
    Module::FunctionListType &functions = llvmMod->getFunctionList();
    for (it=functions.begin(); it!=functions.end(); ++it) {
        Function &f = *it;
        // Disable stack protection
        if (f.hasFnAttribute(Attribute::StackProtect)) {
            //StackProtectStrong
            //StackProtectReq
            LLVMContext &Context = llvmMod->getContext();
            AttributeSet PAL = f.getAttributes();
            PAL = PAL.removeAttribute(Context,
            AttributeSet::FunctionIndex, Attribute::StackProtect);
            f.setAttributes(PAL);
        }
        // Do not instrument functions with a unknown body
        // or the llvm functions (intrinsic)
        if (!f.isDeclaration() && !f.isIntrinsic()) {
            bool isTarget = (targetFun==&f);
            instrumentFunction(&f, isTarget);
        }
    }
    // Validate the generated code, 
    // checking for consistency.
    verifyModule(*llvmMod);
}


//
// Instrument the given function
//
void IRInstrumentor::instrumentFunction(Function *targetFun, bool isTarget) {
    
    //std::cout << "Instrumenting '" 
    //<< targetFun->getName().str() 
    //<< "'... " << std::endl;
    // TODO: Insert the ReportBegin function 
    // at the begining of the main function
    
    // If this function is not the targeted one,
    // insert a pop at the beginning and a push 
    // at the end of this function.
    if (!isTarget) {
        IRBuilder<> IRB(llvmMod->getContext());
        inst_iterator it;
        // Has arguments
        if (targetFun->arg_size()>0) {
            Value *arg1 = IRB.getInt64((intptr_t)targetFun);
            it = inst_begin(targetFun);
            Instruction &first = *it;
            IRB.SetInsertPoint(&first);
            IRB.CreateCall(PopArgsFun, arg1);
        }
        // Does not return void
        if (!targetFun->getReturnType()->isVoidTy()) {
            Value *arg1 = IRB.getInt64((intptr_t)targetFun);
            it = inst_end(targetFun);
            Instruction &last = *--it;
            IRB.SetInsertPoint(&last);
            IRB.CreateCall(PushArgsFun, arg1);
        }
    }
    
    // Replace the assert/assume/exit calls by 
    // by calls to sniper_assert/assume/reportEnd.
    replaceAssertAssumeExitCall(targetFun);
    
    // Iterate through the function
    inst_iterator iit;
    for (iit = inst_begin(targetFun); iit!=inst_end(targetFun); iit++) {
        Instruction &i = *iit;
        switch (i.getOpcode()) {
            case Instruction::Add:
            case Instruction::Sub:
            case Instruction::Mul:
            case Instruction::UDiv:
            case Instruction::SDiv:
            case Instruction::URem:
            case Instruction::SRem:
            case Instruction::And:
            case Instruction::Or:
            case Instruction::Xor:
            case Instruction::ICmp:
                // ExecuteBinaryOp
                insertExecBinOp(&i);
                break;
            case Instruction::SExt:
            case Instruction::ZExt:
                // ExecuteCastOp
                insertExecCastOp(&i);
                break;
            case Instruction::Select:
                // ExecuteSelect
                insertExecSelect(&i);
                break;
            case Instruction::PHI:
                // ExecutePhi
                insertExecPhi(&i);
                break;
            case Instruction::Br:
                // ExecuteCondition
                insertExecBr(&i);
                break;
            case Instruction::Call: {
                // ExecCall
                bool ok = insertExecCall(&i, isTarget);
                // Skip one instruction 
                // (the one we just inserted)
                if (ok) {
                    iit++;
                }
            } break;
            case Instruction::Ret:
                if (isTarget) {
                    // ReportEnd
                    insertReportEnd(&i);
                }
                break;
            case Instruction::Store: 
                insertExecStore(&i);
                break;
            case Instruction::Load:
                insertExecLoad(&i);
                break;
            case Instruction::GetElementPtr:
                insertExecGep(&i);
                break;
            case Instruction::Unreachable:
                //std::cout << "warning: unreachable\n";
                break;
            case Instruction::Alloca:
                insertExecAlloca(&i);
                break;
            case Instruction::BitCast:
                //std::cout << "warning: bitcast\n";
                break;
            case Instruction::FSub:
            case Instruction::FMul:
            case Instruction::FAdd:
            case Instruction::FDiv:
            case Instruction::FRem:
            default:    
                i.dump();
                std::cout << "error: this instruction is not ";
                std::cout << "supported in the Concolic Execution module.\n";
                exit(1);
        }
    }
    // Validate the generated code, 
    // checking for consistency.
    verifyFunction(*targetFun);
}

void IRInstrumentor::insertExecAlloca(Instruction *i) {
    // TODO: Check the type
    // Build arguments
    IRBuilder<> IRB(llvmMod->getContext());
    Value *arg1 = IRB.getInt64((intptr_t)i);
    // Insert before 'i'
    IRB.SetInsertPoint(i);
    IRB.CreateCall(ExecInstFun, arg1);
}

/*void IRInstrumentor::insertExecGep(Instruction *i) {
    GetElementPtrInst *gep = (GetElementPtrInst*) i;
    
    Value *index = NULL;
    Value *ptr = gep->getPointerOperand();
    if(AllocaInst *a = dyn_cast<AllocaInst>(ptr)) {
        // Array (size > 1)
        if(!gep->hasIndices()) {
            error("Pointers not supported!");
        }
        if (gep->getNumIndices()>2) {
            error("Unsupported GetElementPtr instruction!");
        }
        // Check the type and get the index
        Type *ty = a->getAllocatedType();
        int opId = 0;
        if (ty->isIntegerTy()) {
            opId = 1;
        } else if (ty->isArrayTy()) {
            opId = 2;
        } else {
            error("Pointers not supported!");
        }
        index = gep->getOperand(opId);
    } else {
        ptr->dump();
        gep->dump();
         error("Unsupported GetElementPtr instruction!");
    }
    if (!index) {
        error("Pointers not supported!");
    }
    // Build arguments
    IRBuilder<> IRB(llvmMod->getContext());
    Value *arg1 = IRB.getInt64((intptr_t)i);
    // Insert a new call in the IR depending
    // on the type of the argument
    IRB.SetInsertPoint(i);
    Type *T = index->getType();
    if (T->isIntegerTy(32)) {
        IRB.CreateCall2(ExecInst1i32Fun, arg1, index);
    } else if (T->isIntegerTy(64)) {
        // TODO
        //IRB.CreateCall2(ExecInst1i64Fun, arg1, index);
        error("GEP i64 type not supported");
    } else {
        error("only integer/boolean types supported");
    }
}*/

void IRInstrumentor::insertExecGep(Instruction *i) {
    //GetElementPtrInst *gep = (GetElementPtrInst*) i;
    // Build arguments
    IRBuilder<> IRB(llvmMod->getContext());
    Value *arg1 = IRB.getInt64((intptr_t)i);
     // Insert before 'i'
    IRB.SetInsertPoint(i);
    IRB.CreateCall(ExecInstFun, arg1);
}

void IRInstrumentor::insertExecLoad(Instruction *i) {
    // Check the type
    //Value *v = (Value*) i;
    //Type *T = v->getType();
    //if(!T->isIntegerTy()) {
    //    std::cout << "error: only integer/boolean types supported\n";
    //    exit(1);
    //}
    // Build arguments
    LLVMContext &Context = llvmMod->getContext();
    IRBuilder<> IRB(Context);
    Value *arg1 = IRB.getInt64((intptr_t)i);
    LoadInst *load = (LoadInst*) i;
    Value *ptr = load->getPointerOperand();
    Type *destTy = Type::getInt32Ty(Context);
    IRB.SetInsertPoint(i);
    Value *ptrVal = IRB.CreatePtrToInt(ptr, destTy, "sniper_ptrVal");
    // Insert before 'i'
    IRB.SetInsertPoint(i);
    IRB.CreateCall2(ExecInst1i32Fun, arg1, ptrVal);
}

void IRInstrumentor::insertExecStore(Instruction *i) {
    // Check the type
    StoreInst *store = (StoreInst*) i;
    //Value *val = store->getValueOperand();
    //Type *T = val->getType();
    //if(!T->isIntegerTy()) {
    //    std::cout << "error: only integer/boolean types supported\n";
    //    exit(1);
    //}
    // Build arguments
    LLVMContext &Context = llvmMod->getContext();
    IRBuilder<> IRB(Context);
    Value *arg1 = IRB.getInt64((intptr_t)i);
    Value *ptr = store->getPointerOperand();
    Type *destTy = Type::getInt32Ty(Context);
    IRB.SetInsertPoint(i);
    Value *ptrVal = IRB.CreatePtrToInt(ptr, destTy, "sniper_ptrVal");
    // Insert before 'i'
    IRB.SetInsertPoint(i);
    IRB.CreateCall2(ExecInst1i32Fun, arg1, ptrVal);
}

void IRInstrumentor::insertExecCastOp(Instruction *i) {
    // Check type
    //Value *v = (Value*) i;
    //Type *T = v->getType();
    //if(!T->isIntegerTy()) {
    //    std::cout << "error: only integer/boolean types supported\n";
    //    exit(1);
    //}
    // Build arguments
    IRBuilder<> IRB(llvmMod->getContext());
    Value *arg1 = IRB.getInt64((intptr_t)i);
    // Insert before 'i'
    IRB.SetInsertPoint(i);
    IRB.CreateCall(ExecInstFun, arg1);
}

void IRInstrumentor::insertExecSelect(Instruction *i) {
    IRBuilder<> IRB(llvmMod->getContext());
    // Insert before 'i'
    IRB.SetInsertPoint(i);
    // Build arguments
    Value *arg1 = IRB.getInt64((intptr_t)i);
    Value *v1 = i->getOperand(0); // condition
    Value *v2 = i->getOperand(1); // true-value
    Value *v3 = i->getOperand(2); // false-value
    // Insert a new call in the IR depending 
    // on the type of the argument
     // *** TODO: call <v1:bool> <v2:int> <v3:int> *** //
    Type *T = v2->getType();
    if(T->isIntegerTy(1)) {
        IRB.CreateCall4(ExecInst3i1Fun, arg1, v1, v2, v3);
    } else if (T->isIntegerTy(32)) {
        IRB.CreateCall4(ExecInst1i1and2i32Fun, arg1, v1, v2, v3);
    } else {
        assert("Only integer/boolean types are supported!");
    } 
}

void IRInstrumentor::insertExecPhi(Instruction *i) {
    // Get the first instruction that is 
    // after all the phi instructions
    BasicBlock *bb = i->getParent();
    Instruction *firstI = bb->getFirstNonPHI();
    assert(firstI && "Cannot insert sniper_execute for Phi!");
    // Build arguments
    IRBuilder<> IRB(llvmMod->getContext());
    Value *arg1 = IRB.getInt64((intptr_t)i);    
    // Insert after 'i'
    IRB.SetInsertPoint(firstI);
    Value *v = (Value*) i;
    Type *T = v->getType();
    if(T->isIntegerTy(1)) {
        IRB.CreateCall2(ExecInst1i1Fun, arg1, v);
    } else if (T->isIntegerTy(32)) {
        IRB.CreateCall2(ExecInst1i32Fun, arg1, v);
    } /*else if (T->isIntegerTy(64)) {
        IRB.CreateCall2(ExecInst1i64Fun, arg1, v);
    }*/ else {
        assert("Only integer/boolean types are supported!");
    } 
}


bool IRInstrumentor::insertExecCall(Instruction *i, bool isTarget) {
    CallInst *call = (CallInst*) i; 
    Function *calledFun = call->getCalledFunction();
    // Indirect call
    if (!calledFun) {
        calledFun = dyn_cast<Function>(call->getCalledValue()->stripPointerCasts());
        if (!calledFun) {
            std::cout << "warning: unresolvable indirect function call.\n";
            return false;
        }
    }
    // Ignore llvm debug calls
    if(calledFun->isIntrinsic()) {
        return false;
    }
    // Do not process our own calls
    std::string prefix("sniper_");
    std::string name(calledFun->getName().str());
    if(!name.compare(0, prefix.size(), prefix)) {
        return false;
    }
    // Function with an unknown body
    if(calledFun->isDeclaration()) {
        std::cout << "warning: calling a function with no body\n";
        return false;
    }
    // atoi functions are considered as inputs
    if(name=="atoi") {
        // Build arguments
        IRBuilder<> IRB(llvmMod->getContext());
        Value *arg1 = IRB.getInt64((intptr_t)i);
        Value *ptr = IRB.getInt32(0);
        // Insert after 'i'
        BasicBlock::iterator itbb(i);
        ++itbb;
        IRB.SetInsertPoint(itbb);
        // Insert a new call in the IR depending 
        // on the type of the argument
        IRB.CreateCall2(ExecInst1i32Fun, arg1, ptr);
        return true;
    }
    // Check the type of the arguments
    const FunctionType *FTy = calledFun->getFunctionType();
    Function::arg_iterator ait;
    for (ait = calledFun->arg_begin(); ait != calledFun->arg_end(); ++ait) {
        const unsigned argNo = ait->getArgNo();
        const Type *argTy = FTy->getParamType(argNo);
        assert(argTy->isIntegerTy() && "Only integer/boolean types are supported");
    }
    // Check return type
    const Type *retTy = calledFun->getReturnType();
    if (!retTy->isVoidTy() && !retTy->isIntegerTy()) {
        assert("only integer/boolean type supported");
    }
    
    /*
    // Only int type supported (except for atoi functions)
    const Type *argTy = call->getArgOperand(0)->getType();
    if ((!retTy->isIntegerTy()) || !argTy->isIntegerTy()) 
        && name!="atoi") {
        i->dump();
        std::cout << "error: only integer type supported\n";
        exit(1);
    }*/
    
    bool inserted = false;
    IRBuilder<> IRB(llvmMod->getContext());
    // Has arguments
    if (calledFun->arg_size()>0) {
        // Insert a call to the pushArgs function before 'i' 
        Value *arg1 = IRB.getInt64((intptr_t)i);
        IRB.SetInsertPoint(i);
        IRB.CreateCall(PushArgsFun, arg1);  
        inserted = true;
    }
    // Does not return void
    if (!calledFun->getReturnType()->isVoidTy()) {
        // Insert a call to the popArgs function after 'i' 
        BasicBlock::iterator itbb(i);
        ++itbb;
        Instruction *nextInst = itbb;
        if (nextInst==i->getParent()->end()) {
            std::cout << "warning: no next instruction.\n";
            return false;
        }
        Value *arg1 = IRB.getInt64((intptr_t)i);
        IRB.SetInsertPoint(nextInst);
        IRB.CreateCall(PopArgsFun, arg1);
        inserted = true;
    }

    // TODO - summary
    /*
     Value *input = call->getArgOperand(0);
     Value *output = dyn_cast<Value>(i);
     if(input->getType()->isIntegerTy(32) && output->getType()->isIntegerTy(32)) {
        IRB.CreateCall3(ExecInst2i32Fun, arg1, input, output);
    } else {
        std::cout << "error: only integer types supported for functions\n";
        exit(1);
    }*/
    return inserted;
}

void IRInstrumentor::insertExecBinOp(Instruction *i) {
    IRBuilder<> IRB(llvmMod->getContext());
    // Insert before 'i'
    IRB.SetInsertPoint(i);
    // Build arguments
    Value *arg1 = IRB.getInt64((intptr_t)i);
    Value *v1 = i->getOperand(0);
    Value *v2 = i->getOperand(1);
    // Insert a new call in the IR depending 
    // on the type of the argument
    Type *T = v1->getType();
    if(T->isIntegerTy(1)) {
        IRB.CreateCall3(ExecInst2i1Fun, arg1, v1, v2);
    } else if (T->isIntegerTy(32)) {
        IRB.CreateCall3(ExecInst2i32Fun, arg1, v1, v2);
    } else if (T->isIntegerTy(64)) {
        IRB.CreateCall3(ExecInst2i64Fun, arg1, v1, v2);
    } else {
        assert("Only integer/boolean types are supported!");
    } 
}

void IRInstrumentor::insertExecBr(Instruction *i) {
    
    IRBuilder<> IRB(llvmMod->getContext());
    // If the branch is unconditional 
    // we call the function with a true value
    // else, we pass condition value
    BranchInst *br = dyn_cast<BranchInst>(i);
    Value *vcond;
    if(br->isUnconditional()) {
        vcond = IRB.getInt1(true);
    } else {
        vcond = br->getCondition();
    }
    // Build arguments
    Value *arg1 = IRB.getInt64((intptr_t)i);
    // Insert before 'i'
    IRB.SetInsertPoint(i);
    // Insert a new call int the IR 
    IRB.CreateCall2(ExecInst1i1Fun, arg1, vcond);
}


void IRInstrumentor::insertReportEnd(Instruction *i) {
    IRBuilder<> IRB(llvmMod->getContext());
    // Insert before 'i'
    IRB.SetInsertPoint(i);
    // Pass arg to the call the function
    IRB.CreateCall(ReportEndFun);
}

// TODO: check assert/assume fun type (ret void, ret int)
void IRInstrumentor::replaceAssertAssumeExitCall(Function *targetFun) {
    
    // Iterate through the function to save
    // call site of assert and exit function
    std::vector<CallInst*> assumeCalls;
    std::vector<CallInst*> assertCalls;
    std::vector<CallInst*> exitCalls;
    inst_iterator iit;
    for (iit = inst_begin(targetFun); iit!=inst_end(targetFun); iit++) {
        Instruction &i = *iit;
        if (CallInst *c = dyn_cast<CallInst>(&i)) {
            Function *fun = c->getCalledFunction();
            if (!fun) {
                continue;
            }
            StringRef name = fun->getName();
            if (name==Frontend::SNIPER_ASSUME_FUN_NAME) {
                assumeCalls.push_back(c);
            }
            else if (name==Frontend::SNIPER_ASSERT_FUN_NAME) {
                assertCalls.push_back(c);
            } 
            else if (name=="exit") {
                exitCalls.push_back(c);
            }
        } 
    }
    // Replace calls to assume
    // (call sniper_assume(bool) -> call sniper_reportAssume(intptr,bool) )
    IRBuilder<> IRB(llvmMod->getContext());
    std::vector<CallInst*>::iterator it1;
    for (it1=assumeCalls.begin(); it1!=assumeCalls.end(); it1++) {
        CallInst *c = *it1;
        BasicBlock::iterator ii(c);
        std::vector<Value*> args;
        Value *op0 = c->getArgOperand(0);
        Value *addr = NULL;
        // Checking machine's data size
        int n = sizeof(void *);
        assert((n==4 || n==8) && "Unsupported machine's data size!");
        if(n==8) { // 64Bit
            addr = IRB.getInt64((intptr_t)op0);
        } else if (n==4) { // 32Bit
            addr = IRB.getInt32((intptr_t)op0);
        }
        args.push_back(addr);
        args.push_back(op0);
        Function *calledFun = c->getCalledFunction();
        // Indirect call
        if (!calledFun) {
            calledFun =
            dyn_cast<Function>(c->getCalledValue()->stripPointerCasts());
            assert(calledFun && "Unresolvable indirect function call!");
        }
        CallInst *newCall = CallInst::Create(ReportAssumeFun, args);
        // Set debugging metadata
        const DebugLoc dl = c->getDebugLoc();
        newCall->setDebugLoc(dl);
        ReplaceInstWithInst(c->getParent()->getInstList(), ii, newCall);        
    }
    // Replace calls to assert
    // (call sniper_assert() -> call sniper_reportAssert() )
    std::vector<CallInst*>::iterator it2;
    for (it2=assertCalls.begin(); it2!=assertCalls.end(); it2++) {
        CallInst *c = *it2;
        BasicBlock::iterator ii(c);
        std::vector<Value*> args;
        Value *op0 = c->getArgOperand(0);
        Value *addr = NULL;
        // Checking machine's data size
        int n = sizeof(void *);
        assert((n==4 || n==8) && "Unsupported machine's data size!");
        if(n==8) { // 64Bit
            addr = IRB.getInt64((intptr_t)op0);
        } else if (n==4) { // 32Bit
            addr = IRB.getInt32((intptr_t)op0);
        }
        args.push_back(addr);
        args.push_back(op0);
        Function *calledFun = c->getCalledFunction();
        // Indirect call
        if (!calledFun) {
            calledFun =
            dyn_cast<Function>(c->getCalledValue()->stripPointerCasts());
            assert(calledFun && "Unresolvable indirect function call!");
        }
        CallInst *newCall = CallInst::Create(ReportAssertFun, args);
        // Set debugging metadata
        const DebugLoc dl = c->getDebugLoc();
        newCall->setDebugLoc(dl);
        ReplaceInstWithInst(c->getParent()->getInstList(), ii, newCall);
    }
    // Replace calls to exit
    std::vector<CallInst*>::iterator it3;
    for (it3=exitCalls.begin(); it3!=exitCalls.end(); it3++) {
        CallInst *EC = *it3;
        BasicBlock::iterator it(EC);
        CallInst *C = CallInst::Create(ReportEndFun);
        ReplaceInstWithInst(EC->getParent()->getInstList(), it, C);
        it++;
        Instruction *N = it;
        // If the next instruction is a unreach
        // delete it or replace it with a return
        if (isa<UnreachableInst>(N)) {
            N->eraseFromParent();
        }
        // Set debugging metadata
        const DebugLoc D = EC->getDebugLoc();
        C->setDebugLoc(D);
        // Insert a return instruction after the call
        ReturnInst::Create(llvmMod->getContext(), C->getParent());
    }
}